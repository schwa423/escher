// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include "escher/forward_declarations.h"
#include "ftl/macros.h"
#include "ftl/memory/ref_counted.h"

namespace escher {

class Framebuffer : public ftl::RefCountedThreadSafe<Framebuffer> {
 public:
 private:
};

class PaperRenderer : public ftl::RefCountedThreadSafe<PaperRenderer> {
 public:
  class Frame {
   private:
    friend class PaperRenderer;
    PaperRenderer* renderer_;
  };

 private:
  friend class impl::EscherImpl;
  PaperRenderer(impl::EscherImpl* impl);
  ~PaperRenderer();

  impl::EscherImpl* impl_;

  FRIEND_REF_COUNTED_THREAD_SAFE(PaperRenderer);
};

}  // namespace escher
