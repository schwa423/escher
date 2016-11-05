// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <functional>
#include <vector>

#include "escher/forward_declarations.h"
#include "escher/renderer/semaphore_wait.h"
#include "escher/vk/vulkan_context.h"

#include "ftl/macros.h"

namespace escher {
typedef std::function<void()> CommandBufferFinishedCallback;

namespace impl {

// CommandBuffer is a wrapper around vk::CommandBuffer.  Vulkan forbids the
// client application from destroying any resources while they are used by
// any "pending command buffers" (i.e. those that have not finished executing
// on the GPU).
//
// CommandBuffers are obtained from a CommandBufferPool, and is automatically
// returned to it when all GPU-work is finished.
//
// Not thread-safe.
//
// TODO: CommandBuffer was derived from RenderFrame, which it will probably
// subsume (or be used by).
class CommandBuffer {
 public:
  ~CommandBuffer();

  vk::CommandBuffer get() const { return command_buffer_; }

  // Return true if successful.
  bool Submit(vk::Queue queue);

  // During Submit(), these semaphores will be added to the vk::SubmitInfo.
  // No-op if semaphore is null.
  void AddWaitSemaphore(SemaphorePtr semaphore, vk::PipelineStageFlags stage);

  // During Submit(), these semaphores will be added to the vk::SubmitInfo.
  // No-op if semaphore is null.
  void AddSignalSemaphore(SemaphorePtr semaphore);

 private:
  friend class CommandBufferPool;

  // Called by CommandPool, which is responsible for eventually destroying the
  // Vulkan command buffer and fence.  Submit() and Retire() use the fence to
  // determine when the command buffer has finished executing on the GPU.
  CommandBuffer(vk::Device device,
                vk::CommandBuffer command_buffer,
                vk::Fence fence);
  vk::Fence fence() const { return fence_; }

  // Called by CommandPool when obtained from it.
  void Begin(CommandBufferFinishedCallback callback);

  // Called by CommandPool, to attempt to reset the buffer for reuse.  Return
  // false and do nothing if the buffer's submission fence is not ready.
  bool Retire();

  vk::Device device_;
  vk::CommandBuffer command_buffer_;
  vk::Fence fence_;

  std::vector<SemaphorePtr> wait_semaphores_;
  std::vector<vk::PipelineStageFlags> wait_semaphore_stages_;
  std::vector<vk::Semaphore> wait_semaphores_for_submit_;

  std::vector<SemaphorePtr> signal_semaphores_;
  std::vector<vk::Semaphore> signal_semaphores_for_submit_;

  bool is_active_ = false;
  bool is_submitted_ = false;

  CommandBufferFinishedCallback callback_;

  FTL_DISALLOW_COPY_AND_ASSIGN(CommandBuffer);
};

}  // namespace impl
}  // namespace escher
