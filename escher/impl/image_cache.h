// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <queue>
#include <unordered_map>
#include <vulkan/vulkan.hpp>

#include "escher/forward_declarations.h"
#include "escher/renderer/image.h"
#include "escher/renderer/image_factory.h"
#include "escher/resources/resource_manager.h"
#include "escher/util/hash.h"
#include "escher/vk/gpu_mem.h"
#include "ftl/macros.h"
#include "ftl/memory/ref_counted.h"

namespace escher {
namespace impl {

class CommandBufferPool;
class GpuUploader;

// Allow client to obtain new or recycled Images.  All Images obtained from an
// ImageCache must be destroyed before the ImageCache is destroyed.
//
// TODO(ES-7): this does not prune entries!!  Once a new Image is created, it
// will live until the cache is destroyed!!
class ImageCache : public ResourceManager, public ImageFactory {
 public:
  // The allocator is used to allocate memory for newly-created images.  If no
  // allocator is provided, Escher's default allocator is used.
  explicit ImageCache(Escher* escher, GpuAllocator* allocator = nullptr);
  ~ImageCache() override;

  // Obtain an unused Image with the required properties.  A new Image might be
  // created, or an existing one reused.
  ImagePtr NewImage(const ImageInfo& info) override;

 private:
  // Implements Owner::OnReceiveOwnable().  Adds the image to unused_images_.
  void OnReceiveOwnable(std::unique_ptr<Resource> resource) override;

  // Try to find an unused image that meets the required specs.  If successful,
  // remove and return it.  Otherwise, return nullptr.
  ImagePtr FindImage(const ImageInfo& info);

  GpuAllocator* allocator_;

  // Keep track of all images that are available for reuse.
  // TODO: need some method of trimming the cache, to free images that haven't
  // been used.  Note: using a FIFO queue below (instead of a stack) will make
  // this more difficult, but there is a reason to do so: some of these images
  // may still be referenced by a pending command buffer, and reusing them in
  // FIFO order makes it less likely that a pipeline barrier will result in a
  // GPU stall.  The right approach is probably to trim the cache in the most
  // straightforward way, and profile to determine whether GPU stalls are a real
  // concern.
  std::unordered_map<ImageInfo,
                     std::queue<std::unique_ptr<Image>>,
                     Hash<ImageInfo>>
      unused_images_;

  FTL_DISALLOW_COPY_AND_ASSIGN(ImageCache);
};

}  // namespace impl
}  // namespace escher
