#pragma once
// Native UI runtime - macOS window implementation
// Part of the AC6 Recompilation native presenter/window layer

#include <native/ui/window.h>
#include <native/ui/surface_macos.h>

// macOS-specific includes
#include <AppKit/AppKit.h>

namespace rex {
namespace ui {

class MacOSWindow final : public Window {
 public:
  MacOSWindow(WindowedAppContext& app_context, const std::string_view title,
              uint32_t desired_logical_width, uint32_t desired_logical_height);
  ~MacOSWindow() override;

  // Window interface implementation
  bool OpenImpl() override;
  void RequestCloseImpl() override;
  void ApplyNewFullscreen() override;
  void ApplyNewTitle() override;
  void FocusImpl() override;
  std::unique_ptr<Surface> CreateSurfaceImpl(Surface::TypeFlags allowed_types) override;
  void RequestPaintImpl() override;

  // macOS-specific methods
  NSWindow* ns_window() const { return ns_window_; }
  NSView* ns_view() const { return ns_view_; }

 protected:
  void HandleSizeUpdate(WindowDestructionReceiver& destruction_receiver);
  void BeginBatchedSizeUpdate();
  void EndBatchedSizeUpdate(WindowDestructionReceiver& destruction_receiver);

  // Event handlers
  bool HandleMouse(NSEvent* event, WindowDestructionReceiver& destruction_receiver);
  bool HandleKeyboard(NSEvent* event, WindowDestructionReceiver& destruction_receiver);

 private:
  NSWindow* ns_window_ = nullptr;
  NSView* ns_view_ = nullptr;
  NSMenu* ns_menu_ = nullptr;
  int batched_size_update_depth_ = 0;
  bool batched_size_update_contained_configure_ = false;
  bool batched_size_update_contained_draw_ = false;
};

}  // namespace ui
}  // namespace rex
