// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <map>

#include "escher/escher.h"
#include "escher/op/interpreter.h"
#include "paper.capnp.h"

namespace escher {

class PaperInterpreter : public Interpreter {
 public:
  PaperInterpreter(Escher* escher);

 private:
  bool NewMesh(const capnp::NewMesh::Reader& op);
  bool NewMaterial(const capnp::NewMaterial::Reader& op);
  bool DrawList(const capnp::DrawList::Reader& op);

  std::map<uint32_t, MeshPtr> meshes_;

  Table table_;

  Escher* escher_;
};

}  // namespace escher
