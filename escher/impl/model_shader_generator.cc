// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <sstream>

#include "escher/shape/model_shader_generator.h"

namespace escher {
namespace impl {

namespace {

constexpr char g_header[] = R"GLSL(
#version 450
#extension GL_ARB_separate_shader_objects : enable
)GLSL";

constexpr char g_vertex_main[] = R"GLSL(

// Object-space position of the vertex.
vec4 InPosition();

void main() {
  gl_Position =  transform * InPosition();

}
)GLSL";

}  // namespace

ModelShaderSource ModelShaderGenerator::Generate(const PipelineSpec& spec) {
  return ModelShaderSource{GenerateVertexSource(spec),
                           GenerateFragmentSource(spec)};
}

std::string GenerateVertexSource(const PipelineSpec& spec) {
  std::ostringstream ss;
  ss << g_header[] << g_vertex_main;

  const MeshSpec& mesh_spec = spec.mesh_spec;

  // These must be kept in sync with MeshManager::GetMeshSpecImpl().
  if (mesh_spec.flags & MeshAttributeFlagBits::kPosition) {
    ss << "layout(location = 0) in vec2 inPosition;\n";
  }
  if (mesh_spec.flags & MeshAttributeFlagBits::kColor) {
    ss << "layout(location = 1) in vec3 inColor;\n"
       << "layout(location = 1) out vec3 outColor;\n";
  }
  if (mesh_spec.flags & MeshAttributeFlagBits::kUV) {
    ss << "layout(location = 3) in vec2 inUV;\n"
       << "layout(location = 1) out vec3 outColor;\n";
  }
  if (mesh_spec.flags & MeshAttributeFlagBits::kPositionOffset) {
    ss << "layout(location = 4) in vec2 inPositionOffset;\n";
  }

  return ss.str();
}

}  // namespace impl
}  // namespace escher
