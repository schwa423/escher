// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "paper_interpreter.h"

namespace escher {

PaperInterpreter::PaperInterpreter(Escher* escher) : escher_(escher) {
#define CAPNP_TABLE_ENTRY(NAME)                                       \
  Interpreter::Table::Entry {                                         \
    NAME, [this](Op* op) -> bool {                                    \
      auto reader = ::capnp::FlatArrayMessageReader(op->data.data()); \
      return this->NAME(reader.getRoot<capnp::NAME>(reader));         \
    }                                                                 \
  }

  table_.AddEntry(CAPNP_TABLE_ENTRY(NewMesh));
  table_.AddEntry(CAPNP_TABLE_ENTRY(NewMaterial));
  table_.AddEntry(CAPNP_TABLE_ENTRY(DrawList));

#undef CAPNP_TABLE_ENTRY
}

bool PaperInterpreter::NewMesh(const capnp::NewMesh::Reader& op) {
  return false;
}

bool PaperInterpreter::NewMaterial(const capnp::NewMaterial::Reader& op) {
  return false;
}

bool PaperInterpreter::DrawList(const capnp::DrawList::Reader& op) {
  return false;
}

}  // namespace escher
