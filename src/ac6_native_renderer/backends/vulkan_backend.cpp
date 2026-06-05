#include "vulkan_backend.h"

#include <rex/logging.h>
#include "../../d3d_state.h"

namespace ac6::renderer {
namespace {

// Log summary statistics for a captured frame
void LogFrameSummary(const ac6::d3d::FrameCaptureSnapshot& frame_capture) {
  if (frame_capture.draws.empty() && frame_capture.clears.empty() &&
      frame_capture.resolves.empty()) {
    return;
  }

  REXLOG_TRACE("Vulkan native renderer: frame={} draws={} clears={} resolves={}",
               frame_capture.frame_index,
               frame_capture.draws.size(),
               frame_capture.clears.size(),
               frame_capture.resolves.size());

  // Log draw call breakdown
  uint32_t indexed = 0, indexed_shared = 0, primitive = 0;
  for (const auto& draw : frame_capture.draws) {
    switch (draw.kind) {
      case ac6::d3d::DrawCallKind::kIndexed:
        ++indexed;
        break;
      case ac6::d3d::DrawCallKind::kIndexedShared:
        ++indexed_shared;
        break;
      case ac6::d3d::DrawCallKind::kPrimitive:
        ++primitive;
        break;
    }
  }

  if (!frame_capture.draws.empty()) {
    REXLOG_TRACE("  Draw breakdown: indexed={} indexed_shared={} primitive={}",
                 indexed, indexed_shared, primitive);
  }
}

}  // namespace

bool VulkanBackend::IsSupported() const {
#if defined(__linux__)
  return true;
#else
  return false;
#endif
}

bool VulkanBackend::Initialize(const NativeRendererConfig& config) {
  (void)config;
  if (initialized_) {
    return true;
  }

  // Phase-1 scaffold: we deliberately do not create Vulkan resources yet,
  // to avoid conflicting with the existing Rexglue Xenos provider during
  // parallel bring-up.

  // The game renders through ReXGlue's built-in Xenos GPU emulation
  // (Vulkan backend). This native renderer backend is a placeholder
  // that captures rendering state for future use.

  initialized_ = true;
  REXLOG_INFO("AC6 native renderer Vulkan backend initialized (scaffold - game renders via Rexglue Xenos emulation)");
  return true;
}

void VulkanBackend::Shutdown() {
  if (!initialized_) {
    return;
  }
  initialized_ = false;
}

void VulkanBackend::RecordFrame(const ac6::d3d::FrameCaptureSnapshot& frame_capture) {
  if (!initialized_) {
    return;
  }

  // Log frame summary for debugging
  LogFrameSummary(frame_capture);

  // Phase-1: do not execute native rendering.
  // The game renders through ReXGlue's built-in Xenos GPU emulation.
  //
  // To implement a full native renderer, you would need to:
  //
  // 1. Create Vulkan resources (instance, device, swapchain, command buffers)
  // 2. Map guest memory addresses to host memory via ReXGlue's memory system:
  //    - Vertex buffers: guest address -> host pointer via rex::memory
  //    - Index buffers: guest address -> host pointer
  //    - Texture data: guest address -> host pointer
  // 3. Translate Xenos/D3D resource types to Vulkan equivalents:
  //    - Surface formats: Xenos format enum -> VkFormat
  //    - Texture views: guest texture pointer -> VkImageView
  //    - Render targets: guest surface pointer -> VkImageView
  // 4. Record Vulkan command buffers:
  //    - Clear operations: vkCmdClearAttachments
  //    - Draw operations: vkCmdBindVertexBuffers, vkCmdBindIndexBuffer, vkCmdDraw
  //    - Resolve operations: vkCmdResolveImage
  // 5. Submit and present frames

  // For debugging, log the first few draw calls
  if (!frame_capture.draws.empty()) {
    const auto& first_draw = frame_capture.draws.front();
    REXLOG_TRACE("  First draw: kind={} prim={} start={} count={}",
                 static_cast<int>(first_draw.kind),
                 first_draw.primitive_type,
                 first_draw.start,
                 first_draw.count);
  }
}

}  // namespace ac6::renderer
