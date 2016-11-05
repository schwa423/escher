// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include "escher/shape/pipeline_spec.h"

namespace escher {
namespace impl {

struct ModelShaderSource {
  std::string vertex_src;
  std::string fragment_src;
};

class ModelShaderGenerator {
  ModelShaderSource Generate(const PipelineSpec& spec);
};

}  // namespace impl
}  // namespace escher
