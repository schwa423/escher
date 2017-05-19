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

constexpr char g_per_model_uniforms[] = R"GLSL(
layout(set = 0, binding = 0) uniform PerModel {
  vec2 frag_coord_to_uv_multiplier;
  float time;
};

)GLSL";

constexpr char g_per_object_basic_uniforms[] = R"GLSL(
layout(set = 1, binding = 0) uniform PerObject {
  mat4 transform;
  vec4 color;
};

)GLSL";

constexpr char g_per_object_wobble_uniforms[] = R"GLSL(
layout(set = 1, binding = 0) uniform PerObject {
  mat4 transform;
  vec4 color;
  // Corresponds to ModifierWobble::SineParams[0].
  float speed_0;
  float amplitude_0;
  float frequency_0;
  // Corresponds to ModifierWobble::SineParams[1].
  float speed_1;
  float amplitude_1;
  float frequency_1;
  // Corresponds to ModifierWobble::SineParams[2].
  float speed_2;
  float amplitude_2;
  float frequency_2;
  // TODO: for some reason, I can't say:
  //   SineParams sine_params[kNumSineParams];
  // nor:
  //   SineParams sine_params_0;
  //   SineParams sine_params_1;
  //   SineParams sine_params_2;
  // ... if I try, the GLSL compiler produces SPIR-V, but the "SC"
  // validation layer complains when trying to create a vk::ShaderModule
  // from that SPIR-V.  Note: if we ignore the warning and proceed, nothing
  // explodes.  Nevertheless, we'll leave it this way for now, to be safe.
};

)GLSL";

constexpr char g_vertex_main[] = R"GLSL(
// Object-space position of the vertex.
vec4 InPosition();

// Write values that should be interpolated by the rasterizer.
void WriteVaryingOutputs();

void main() {
  gl_Position = transform * InPosition();
  WriteVaryingOutputs();
}

)GLSL";

}  // namespace

ModelShaderSource ModelShaderGenerator::Generate(const PipelineSpec& spec) {
  return ModelShaderSource{GenerateVertexSource(spec),
                           GenerateFragmentSource(spec)};
}

std::string GenerateVertexSource(const PipelineSpec& spec) {
  std::ostringstream ss;
  ss << g_header << g_per_object;

  // Subsequent code must be kept in sync with MeshManager::GetMeshSpecImpl().
  const MeshSpec& mesh_spec = spec.mesh_spec;

  std::string in_position_src;
  if (mesh_spec.flags & MeshAttributeFlagBits::kPosition) {
    ss << "layout(location = 0) in vec2 in_position;\n";

    if (mesh_spec.flags & MeshAttributeFlagBits::kPositionOffset) {
      ss << "layout(location = 1) in vec2 in_position_offset;\n";
      in_position_src =
          "vec4 InPosition() { return in_position + in_position_offset; }";
    } else {
      in_position_src = "vec4 InPosition() { return in_position; }";
    }
  } else {
    // Position-offset only makes sense if a position is available.
    FTL_DCHECK(!(mesh_spec.flags & MeshAttributeFlagBits::kPositionOffset));
    // Eventually there might be a way to generate vertex positions without
    // an explicit position attribute (e.g. by evaluating a bezier patch, or
    // reading from a "geometry texture"), but there currently is not.
    FTL_DCHECK(false);
  }

  std::string write_varying_outputs_src;
  if (mesh_spec.flags & MeshAttributeFlagBits::kUV) {
    ss << "layout(location = 2) in vec2 in_uv;\n"
       << "layout(location = 2) out vec2 out_uv;\n";

    write_varying_outputs_src =
        "void WriteVaryingOutputs() { out_uv = in_uv; }";
  } else {
    write_varying_outputs_src =
        "void WriteVaryingOutputs() { return; }  // no-op";
  }

  ss << g_vertex_main << in_position_src << "\n\n" << write_varying_outputs_src;
  return ss.str();
}

std::string GenerateFragmentSource(const PipelineSpec& spec) {
  std::ostringstream ss;
  ss << g_header << g_per_model << g_per_object;

  // Subsequent code must be kept in sync with MeshManager::GetMeshSpecImpl().
  const MeshSpec& mesh_spec = spec.mesh_spec;
}

}  // namespace impl
}  // namespace escher
