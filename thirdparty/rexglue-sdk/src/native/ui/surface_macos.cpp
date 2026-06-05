// Native UI runtime - macOS surface implementation
// Part of the AC6 Recompilation native presenter/window layer

#include <native/ui/surface_macos.h>

namespace rex {
namespace ui {

bool MacOSNSViewSurface::GetSizeImpl(uint32_t& width_out, uint32_t& height_out) const {
  if (!ns_view_) {
    return false;
  }
  
  NSRect frame = [ns_view_ frame];
  width_out = uint32_t(frame.size.width);
  height_out = uint32_t(frame.size.height);
  return true;
}

}  // namespace ui

}  // namespace rex
