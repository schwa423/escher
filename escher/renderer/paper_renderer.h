// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include "escher/forward_declarations.h"
#include "escher/renderer/renderer.h"

namespace escher {

class PaperRenderer : public Renderer {
 public:
  void DrawFrame(Stage& stage,
                 Model& model,
                 const ImagePtr& color_image_out,
                 const SemaphorePtr& frame_done,
                 FrameRetiredCallback frame_retired_callback) override;

  // Set whether one or more debug-overlays is to be show.
  void set_show_debug_info(bool b) { show_debug_info_ = b; }

  // Set whether SSDO lighting model is used.
  void set_enable_lighting(bool b) { enable_lighting_ = b; }

 private:
  friend class Escher;
  PaperRenderer(impl::EscherImpl* escher);
  ~PaperRenderer() override;

  static constexpr uint32_t kFramebufferColorAttachmentIndex = 0;
  static constexpr uint32_t kFramebufferDepthAttachmentIndex = 1;

  // Render pass that generates a depth buffer, but no color fragments.  The
  // resulting depth buffer is used by DrawSsdoPasses() in order to compute
  // per-pixel occlusion, and by DrawLightingPass().
  void DrawDepthPrePass(const FramebufferPtr& framebuffer,
                        Stage& stage,
                        Model& model);

  // Multiple render passes.  The first samples the depth buffer to generate
  // per-pixel occlusion information, and subsequent passes filter this noisy
  // data.
  void DrawSsdoPasses(const TexturePtr& depth_in,
                      const ImagePtr& color_out,
                      const ImagePtr& color_aux,
                      Stage& stage);

  // Render pass that renders the fully-lit/shadowed scene.  Uses the depth
  // buffer from DrawDepthPrePass(), and the illumination texture from
  // DrawSsdoPasses().
  // TODO: on GPUs that use tiled rendering, it may be faster simply clear the
  // depth buffer instead of reusing the values from DrawDepthPrePass().  This
  // might save bandwidth at the cost of more per-fragment computation (but
  // the latter might be mitigated by sorting front-to-back, etc.).  Revisit
  // after doing performance profiling.
  void DrawLightingPass(const FramebufferPtr& framebuffer,
                        const TexturePtr& illumination_texture,
                        Stage& stage,
                        Model& model);

  void DrawDebugOverlays(const ImagePtr& output,
                         const ImagePtr& depth,
                         const ImagePtr& illumination);

  // Configure the renderer to use the specified output formats.
  void UpdateModelRenderer(vk::Format pre_pass_color_format,
                           vk::Format lighting_pass_color_format);

  MeshPtr full_screen_;
  impl::ImageCache* image_cache_;
  vk::Format depth_format_;
  std::unique_ptr<impl::ModelData> model_data_;
  std::unique_ptr<impl::ModelRenderer> model_renderer_;
  std::unique_ptr<impl::SsdoSampler> ssdo_;
  std::vector<vk::ClearValue> clear_values_;
  bool show_debug_info_ = false;
  bool enable_lighting_ = true;

  FRIEND_REF_COUNTED_THREAD_SAFE(PaperRenderer);
  FTL_DISALLOW_COPY_AND_ASSIGN(PaperRenderer);
};

}  // namespace escher
