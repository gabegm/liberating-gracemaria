// Native UI runtime - macOS window implementation
// Part of the AC6 Recompilation native presenter/window layer

#include <algorithm>
#include <string>

#include <rex/assert.h>
#include <rex/cvar.h>
#include <rex/graphics/flags.h>
#include <rex/graphics/video_mode_util.h>
#include <rex/logging.h>
#include <rex/platform.h>
#include <rex/ui/flags.h>
#include <rex/ui/virtual_key.h>
#include <native/ui/window_macos.h>

#include <native/ui/surface_macos.h>

namespace {

uint32_t ResolveWindowWidth(uint32_t requested_width) {
  if (REXCVAR_GET(window_width) > 0) {
    return uint32_t(REXCVAR_GET(window_width));
  }
  if (!rex::cvar::HasNonDefaultValue("window_width")) {
    if (rex::cvar::HasNonDefaultValue("video_mode_width") && REXCVAR_GET(video_mode_width) > 0) {
      return uint32_t(std::clamp(REXCVAR_GET(video_mode_width), 1, 8192));
    }
    int32_t preset_width = 0;
    int32_t preset_height = 0;
    if (rex::graphics::video_mode_util::TryGetResolutionPresetFromCVar(preset_width,
                                                                       preset_height)) {
      return uint32_t(std::clamp(preset_width, 1, 8192));
    }
  }
  return requested_width;
}

uint32_t ResolveWindowHeight(uint32_t requested_height) {
  if (REXCVAR_GET(window_height) > 0) {
    return uint32_t(REXCVAR_GET(window_height));
  }
  if (!rex::cvar::HasNonDefaultValue("window_height")) {
    if (rex::cvar::HasNonDefaultValue("video_mode_height") && REXCVAR_GET(video_mode_height) > 0) {
      return uint32_t(std::clamp(REXCVAR_GET(video_mode_height), 1, 8192));
    }
    int32_t preset_width = 0;
    int32_t preset_height = 0;
    if (rex::graphics::video_mode_util::TryGetResolutionPresetFromCVar(preset_width,
                                                                       preset_height)) {
      return uint32_t(std::clamp(preset_height, 1, 8192));
    }
  }
  return requested_height;
}

rex::ui::VirtualKey TranslateNSEventKeyboardEventToVirtualKey(NSEvent* event) {
  // This is a simplified mapping. A full implementation would need to handle
  // all macOS key codes properly.
  return rex::ui::VirtualKey::kNone;
}

}  // namespace

namespace rex {
namespace ui {

std::unique_ptr<Window> Window::Create(WindowedAppContext& app_context,
                                       const std::string_view title, uint32_t desired_logical_width,
                                       uint32_t desired_logical_height) {
  desired_logical_width = ResolveWindowWidth(desired_logical_width);
  desired_logical_height = ResolveWindowHeight(desired_logical_height);
  return std::make_unique<MacOSWindow>(app_context, title, desired_logical_width,
                                       desired_logical_height);
}

MacOSWindow::MacOSWindow(WindowedAppContext& app_context, const std::string_view title,
                         uint32_t desired_logical_width, uint32_t desired_logical_height)
    : Window(app_context, title, desired_logical_width, desired_logical_height) {}

MacOSWindow::~MacOSWindow() {
  EnterDestructor();
  if (ns_window_) {
    // Set ns_window_ to null to ignore events from now on since this ui::MacOSWindow
    // is entering an indeterminate state.
    NSWindow* window = ns_window_;
    ns_window_ = nullptr;
    ns_view_ = nullptr;
    [window close];
  }
}

bool MacOSWindow::OpenImpl() {
  // Create the macOS window
  NSRect window_rect = NSMakeRect(0, 0, GetDesiredLogicalWidth(), GetDesiredLogicalHeight());
  NSWindowStyleMask style = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                            NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable;
  
  ns_window_ = [[NSWindow alloc] initWithContentRect:window_rect
                                           styleMask:style
                                             backing:NSBackingStoreBuffered
                                               defer:NO];
  
  [ns_window_ setTitle:GetTitle().c_str()];
  [ns_window_ setReleasedWhenClosed:NO];
  
  // Create the view for the window
  NSRect view_rect = [ns_window_ contentRectForFrameRect:[ns_window_ frame]];
  ns_view_ = [[NSView alloc] initWithFrame:view_rect];
  [ns_window_ setContentView:ns_view_];
  
  // Show the window
  [ns_window_ orderFrontRegardless];
  
  // Enter fullscreen if requested
  if (IsFullscreen()) {
    [ns_window_ toggleFullScreen:nil];
  }
  
  // Report initial size
  {
    WindowDestructionReceiver destruction_receiver(this);
    NSRect content_rect = [ns_window_ contentRectForFrameRect:[ns_window_ frame]];
    OnActualSizeUpdate(uint32_t(content_rect.size.width),
                       uint32_t(content_rect.size.height), destruction_receiver);
    if (destruction_receiver.IsWindowDestroyedOrClosed()) {
      return true;
    }
  }
  
  return true;
}

void MacOSWindow::RequestCloseImpl() {
  if (ns_window_) {
    [ns_window_ close];
  }
}

void MacOSWindow::ApplyNewFullscreen() {
  WindowDestructionReceiver destruction_receiver(this);
  
  if (IsFullscreen()) {
    [ns_window_ toggleFullScreen:nil];
  } else {
    [ns_window_ toggleFullScreen:nil];
  }
  
  EndBatchedSizeUpdate(destruction_receiver);
}

void MacOSWindow::ApplyNewTitle() {
  if (ns_window_) {
    [ns_window_ setTitle:GetTitle().c_str()];
  }
}

void MacOSWindow::FocusImpl() {
  if (ns_window_) {
    [ns_window_ makeKeyAndOrderFront:nil];
  }
}

std::unique_ptr<Surface> MacOSWindow::CreateSurfaceImpl(Surface::TypeFlags allowed_types) {
  if (allowed_types & Surface::kTypeFlag_MacOSNSView) {
    return std::make_unique<MacOSNSViewSurface>(ns_view_);
  }
  return nullptr;
}

void MacOSWindow::RequestPaintImpl() {
  if (ns_view_) {
    [ns_view_ setNeedsDisplay:YES];
  }
}

void MacOSWindow::HandleSizeUpdate(WindowDestructionReceiver& destruction_receiver) {
  if (!ns_view_) {
    return;
  }
  
  NSRect content_rect = [ns_window_ contentRectForFrameRect:[ns_window_ frame]];
  OnActualSizeUpdate(uint32_t(content_rect.size.width),
                     uint32_t(content_rect.size.height), destruction_receiver);
}

void MacOSWindow::BeginBatchedSizeUpdate() {
  ++batched_size_update_depth_;
}

void MacOSWindow::EndBatchedSizeUpdate(WindowDestructionReceiver& destruction_receiver) {
  if (--batched_size_update_depth_) {
    return;
  }
  
  if (batched_size_update_contained_configure_) {
    batched_size_update_contained_configure_ = false;
    HandleSizeUpdate(destruction_receiver);
  }
  if (batched_size_update_contained_draw_) {
    batched_size_update_contained_draw_ = false;
    RequestPaint();
  }
}

bool MacOSWindow::HandleMouse(NSEvent* event, WindowDestructionReceiver& destruction_receiver) {
  // Simplified mouse handling
  return false;
}

bool MacOSWindow::HandleKeyboard(NSEvent* event, WindowDestructionReceiver& destruction_receiver) {
  // Simplified keyboard handling
  return false;
}

}  // namespace ui

}  // namespace rex
