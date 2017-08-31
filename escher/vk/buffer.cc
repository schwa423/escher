// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "escher/vk/buffer.h"

#include "escher/impl/vulkan_utils.h"
#include "escher/resources/resource_manager.h"
#include "escher/vk/gpu_allocator.h"

namespace escher {

const ResourceTypeInfo Buffer::kTypeInfo("Buffer",
                                         ResourceType::kResource,
                                         ResourceType::kWaitableResource,
                                         ResourceType::kBuffer);

BufferPtr Buffer::New(ResourceManager* manager,
                      GpuAllocator* allocator,
                      vk::DeviceSize size,
                      vk::BufferUsageFlags usage_flags,
                      vk::MemoryPropertyFlags memory_property_flags) {
  auto device = manager->vulkan_context().device;

  // Create buffer.
  vk::BufferCreateInfo buffer_create_info;
  buffer_create_info.size = size;
  buffer_create_info.usage = usage_flags;
  buffer_create_info.sharingMode = vk::SharingMode::eExclusive;
  auto vk_buffer =
      ESCHER_CHECKED_VK_RESULT(device.createBuffer(buffer_create_info));

  // Allocate memory for the buffer.
  auto mem = allocator->Allocate(device.getBufferMemoryRequirements(vk_buffer),
                                 memory_property_flags);

  return ftl::MakeRefCounted<Buffer>(manager, std::move(mem), vk_buffer, size);
}

BufferPtr Buffer::New(ResourceManager* manager,
                      GpuMemPtr mem,
                      vk::BufferUsageFlags usage_flags,
                      vk::DeviceSize size,
                      vk::DeviceSize offset) {
  auto device = manager->vulkan_context().device;

  // Create buffer.
  vk::BufferCreateInfo buffer_create_info;
  buffer_create_info.size = size;
  buffer_create_info.usage = usage_flags;
  buffer_create_info.sharingMode = vk::SharingMode::eExclusive;
  auto vk_buffer =
      ESCHER_CHECKED_VK_RESULT(device.createBuffer(buffer_create_info));

  return ftl::MakeRefCounted<Buffer>(manager, std::move(mem), vk_buffer, size);
}

Buffer::Buffer(ResourceManager* manager,
               GpuMemPtr mem,
               vk::Buffer buffer,
               vk::DeviceSize size,
               vk::DeviceSize offset)
    : WaitableResource(manager),
      mem_(std::move(mem)),
      buffer_(buffer),
      size_(size),
      ptr_(mem_->mapped_ptr()) {
  FTL_DCHECK(size + offset <= mem_->size());
  vulkan_context().device.bindBufferMemory(buffer_, mem_->base(),
                                           mem_->offset() + offset);
}

Buffer::~Buffer() {
  vulkan_context().device.destroyBuffer(buffer_);
}

}  // namespace escher
