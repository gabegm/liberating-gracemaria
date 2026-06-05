#pragma once

#include "../render_device.h"

// Forward declarations from d3d_state.h
namespace ac6::d3d {
struct FrameCaptureSnapshot;
}  // namespace ac6::d3d

namespace ac6::renderer {

// ============================================================================
// Vulkan backend - Phase 1 scaffold
//
// This backend captures rendering state from the recompiled PPC code but
// does not yet execute native rendering. The game renders through
// ReXGlue's built-in Xenos GPU emulation (Vulkan backend).
//
// To implement a full native renderer, you would need to:
// 1. Create Vulkan instance, device, swapchain
// 2. Map guest memory addresses to host memory via ReXGlue's memory system
// 3. Translate D3D/Xenos resource types to Vulkan equivalents
// 4. Record Vulkan command buffers for clears, draws, and resolves
// 5. Submit and present frames
//
// See vulkan_backend.cpp for the scaffold implementation.
// ============================================================================

class VulkanBackend final : public RenderDeviceBackend {
 public:
  BackendType GetType() const override { return BackendType::kVulkan; }
  std::string_view GetName() const override { return "vulkan"; }
  bool IsSupported() const override;
  bool Initialize(const NativeRendererConfig& config) override;
  void Shutdown() override;

  // Record and execute a captured frame (scaffold - does nothing)
  void RecordFrame(const ac6::d3d::FrameCaptureSnapshot& frame_capture);

 private:
  bool initialized_ = false;
};

}  // namespace ac6::renderer
