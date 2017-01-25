// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <cmath>
#include <iostream>

#include "demo.h"

#include "escher/escher.h"
#include "escher/escher_process_init.h"
#include "escher/examples/waterfall/scenes/wobbly_ocean_scene.h"
#include "escher/examples/waterfall/scenes/ring_tricks3.h"
#include "escher/examples/waterfall/scenes/ring_tricks2.h"
#include "escher/examples/waterfall/scenes/ring_tricks1.h"
#include "escher/examples/waterfall/scenes/uber_scene3.h"
#include "escher/examples/waterfall/scenes/uber_scene2.h"
#include "escher/examples/waterfall/scenes/uber_scene.h"
#include "escher/examples/waterfall/scenes/demo_scene.h"
#include "escher/examples/waterfall/scenes/wobbly_rings_scene.h"
#include "escher/geometry/types.h"
#include "escher/material/color_utils.h"
#include "escher/renderer/paper_renderer.h"
#include "escher/scene/stage.h"
#include "escher/util/stopwatch.h"
#include "escher/vk/vulkan_swapchain_helper.h"
#include "ftl/logging.h"

static constexpr int kDemoWidth = 2160;
static constexpr int kDemoHeight = 1440;

// Material design places objects from 0.0f to 24.0f.
static constexpr float kNear = 100.f;
static constexpr float kFar = 0.f;

bool g_show_debug_info = false;
bool g_enable_lighting = true;
int g_current_scene = 0;
bool g_profile_one_frame = false;

std::unique_ptr<Demo> CreateDemo(bool use_fullscreen) {
  Demo::WindowParams window_params;
  window_params.window_name = "Escher Waterfall Demo (Vulkan)";
  window_params.width = kDemoWidth;
  window_params.height = kDemoHeight;
  window_params.desired_swapchain_image_count = 2;
  window_params.use_fullscreen = use_fullscreen;

  Demo::InstanceParams instance_params;

  if (use_fullscreen) {
    FTL_LOG(INFO) << "Using fullscreen window: " << window_params.width << "x"
                  << window_params.height;
  } else {
    FTL_LOG(INFO) << "Using 'windowed' window: " << window_params.width << "x"
                  << window_params.height;
  }

  return std::make_unique<Demo>(instance_params, window_params);
}

int main(int argc, char** argv) {
  bool use_fullscreen = false;
  for (int i = 1; i < argc; ++i) {
    if (!strcmp("--fullscreen", argv[i])) {
      use_fullscreen = true;
    }
  }

  auto demo = CreateDemo(use_fullscreen);

  demo->SetKeyCallback("ESCAPE", [&]() { demo->SetShouldQuit(); });
  demo->SetKeyCallback("SPACE",
                       [&]() { g_enable_lighting = !g_enable_lighting; });
  demo->SetKeyCallback("D", [&]() { g_show_debug_info = !g_show_debug_info; });
  demo->SetKeyCallback("P", [&]() { g_profile_one_frame = true; });
  demo->SetKeyCallback("1", [&]() { g_current_scene = 1; });
  demo->SetKeyCallback("2", [&]() { g_current_scene = 2; });
  demo->SetKeyCallback("3", [&]() { g_current_scene = 3; });
  demo->SetKeyCallback("4", [&]() { g_current_scene = 4; });
  demo->SetKeyCallback("5", [&]() { g_current_scene = 5; });
  demo->SetKeyCallback("6", [&]() { g_current_scene = 6; });
  demo->SetKeyCallback("7", [&]() { g_current_scene = 7; });
  demo->SetKeyCallback("8", [&]() { g_current_scene = 8; });
  demo->SetKeyCallback("9", [&]() { g_current_scene = 9; });
  demo->SetKeyCallback("0", [&]() { g_current_scene = 10; });

  escher::GlslangInitializeProcess();
  {
    escher::VulkanContext vulkan_context = demo->GetVulkanContext();

    escher::Escher escher(vulkan_context);
    auto renderer = escher.NewPaperRenderer();
    escher::VulkanSwapchainHelper swapchain_helper(demo->GetVulkanSwapchain(),
                                                   renderer);

    escher::vec2 focus;
    escher::Stage stage;
    stage.Resize(escher::SizeI(kDemoWidth, kDemoHeight), 1.0,
                 escher::SizeI(0, 0));
    stage.set_viewing_volume(
        escher::ViewingVolume(kDemoWidth, kDemoHeight, kNear, kFar));
    // TODO: perhaps lights should be initialized by the various demo scenes.
    stage.set_key_light(escher::DirectionalLight(
        escher::vec2(1.5f * M_PI, 1.5f * M_PI), 0.15f * M_PI, 1.f));
    stage.set_fill_light(escher::AmbientLight(0.3f));

    escher::Stopwatch stopwatch;
    uint64_t frame_count = 0;
    uint64_t first_frame_microseconds;

    // Create list of scenes
    std::vector<std::unique_ptr<Scene>> scenes;

    scenes.emplace_back(new RingTricks2(&vulkan_context, &escher));
    scenes.emplace_back(new UberScene3(&vulkan_context, &escher));
    scenes.emplace_back(new WobblyOceanScene(&vulkan_context, &escher));
    scenes.emplace_back(new WobblyRingsScene(
        &vulkan_context, &escher, vec3(0.012, 0.047, 0.427),
        vec3(0.929f, 0.678f, 0.925f), vec3(0.259f, 0.956f, 0.667),
        vec3(0.039f, 0.788f, 0.788f), vec3(0.188f, 0.188f, 0.788f),
        vec3(0.588f, 0.239f, 0.729f)));
    scenes.emplace_back(new UberScene2(&vulkan_context, &escher));
    scenes.emplace_back(new RingTricks3(&vulkan_context, &escher));
    // scenes.emplace_back(new RingTricks1(&vulkan_context, &escher));

    const int kNumColorsInScheme = 4;
    vec3 color_schemes[4][kNumColorsInScheme]{
        {vec3(0.565, 0.565, 0.560), vec3(0.868, 0.888, 0.438),
         vec3(0.905, 0.394, 0.366), vec3(0.365, 0.376, 0.318)},
        {vec3(0.299, 0.263, 0.209), vec3(0.986, 0.958, 0.553),
         vec3(0.773, 0.750, 0.667), vec3(0.643, 0.785, 0.765)},
        {vec3(0.171, 0.245, 0.120), vec3(0.427, 0.458, 0.217),
         vec3(0.750, 0.736, 0.527), vec3(0.366, 0.310, 0.280)},
        {vec3(0.170, 0.255, 0.276), vec3(0.300, 0.541, 0.604),
         vec3(0.637, 0.725, 0.747), vec3(0.670, 0.675, 0.674)},
    };
    for (auto& color_scheme : color_schemes) {
      // Convert colors from sRGB
      for (int i = 0; i < kNumColorsInScheme; i++) {
        color_scheme[i] = escher::SrgbToLinear(color_scheme[i]);
      }

      // Create a new scheme with each color scheme
      scenes.emplace_back(new WobblyRingsScene(
          &vulkan_context, &escher, color_scheme[0], color_scheme[1],
          color_scheme[1], color_scheme[1], color_scheme[2], color_scheme[3]));
    }
    for (auto& scene : scenes) {
      scene->Init(&stage);
    }

    while (!demo->ShouldQuit()) {
      g_current_scene = g_current_scene % scenes.size();
      auto& scene = scenes.at(g_current_scene);
      escher::Model* model = scene->Update(stopwatch, frame_count, &stage);

      renderer->set_show_debug_info(g_show_debug_info);
      renderer->set_enable_lighting(g_enable_lighting);
      renderer->set_enable_profiling(g_profile_one_frame);
      g_profile_one_frame = false;

      swapchain_helper.DrawFrame(stage, *model);

      if (++frame_count == 1) {
        first_frame_microseconds = stopwatch.GetElapsedMicroseconds();
        stopwatch.Reset();
      }

      demo->PollEvents();
    }

    vulkan_context.device.waitIdle();

    auto microseconds = stopwatch.GetElapsedMicroseconds();
    double fps = (frame_count - 1) * 1000000.0 / microseconds;
    FTL_LOG(INFO) << "Average frame rate: " << fps;
    FTL_LOG(INFO) << "First frame took: " << first_frame_microseconds / 1000.0
                  << " milliseconds";
  }
  escher::GlslangFinalizeProcess();

  return 0;
}
