// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "escher/paper/paper_renderer.h"

#include "escher/impl/escher_impl.h"

namespace escher {

PaperRenderer::PaperRenderer(impl::EscherImpl* impl) {
  impl_->IncrementRendererCount();
}

PaperRenderer::~PaperRenderer() {
  impl_->DecrementRendererCount();
}

}  // namespace escher
