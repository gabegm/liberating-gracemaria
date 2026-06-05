#pragma once
// Native UI runtime - macOS surface abstraction
// Part of the AC6 Recompilation native presenter/window layer

#include <cstdint>
#include <memory>

#include <native/ui/surface.h>

// macOS-specific includes
#include <AppKit/AppKit.h>

namespace rex {
namespace ui {

class MacOSNSViewSurface final : public Surface {
 public:
  explicit MacOSNSViewSurface(NSView* ns_view) : ns_view_(ns_view) {}
  TypeIndex GetType() const override { return kTypeIndex_MacOSNSView; }
  NSView* ns_view() const { return ns_view_; }

 protected:
  bool GetSizeImpl(uint32_t& width_out, uint32_t& height_out) const override;

 private:
  NSView* ns_view_;
};

}  // namespace ui
}  // namespace rex
