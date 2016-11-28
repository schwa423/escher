// Copyright 2017 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <vector>

#include "escher/forward_declarations.h"
#include "escher/vk/vulkan_context.h"
#include "ftl/memory/ref_counted.h"

namespace escher {

class TimestampProfiler : public ftl::RefCountedThreadSafe<TimestampProfiler> {
 public:
  TimestampProfiler(vk::Device device);
  ~TimestampProfiler();

  void AddTimestamp(impl::CommandBuffer* cmd_buf,
                    vk::PipelineStageFlagBits flags,
                    std::string name);

  struct Result {
    uint64_t time;
    std::string name;
  };

  std::vector<Result> GetQueryResults();

  static bool SupportsQueueFamily(const vk::QueueFamilyProperties& props);

 private:
  struct QueryRange {
    vk::QueryPool pool;
    vk::CommandBuffer command_buffer;
    uint32_t start_index;  // within the pool
    uint32_t count;
  };

  QueryRange* ObtainRange(impl::CommandBuffer* cmd_buf);
  QueryRange* CreateRange(impl::CommandBuffer* cmd_buf);
  QueryRange* CreateRangeAndPool(impl::CommandBuffer* cmd_buf);

  std::vector<QueryRange> ranges_;
  std::vector<vk::QueryPool> pools_;

  // Remembers timestamp names, and will eventually be filled in with timestamp
  // values before returning results.
  std::vector<Result> results_;

  uint32_t query_count_ = 0;
  uint32_t current_pool_index_ = 0;

  vk::Device device_;
};

}  // namespace escher
