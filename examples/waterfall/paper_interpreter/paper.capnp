# Copyright 2016 The Fuchsia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

@0xfcffe355ed8c05e9;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("escher::capnp");

struct MeshVertex {
  x @0 : Float32;
  y @1 : Float32;
  r @2 : Float32;
  g @3 : Float32;
  b @4 : Float32;
}

struct NewMesh {
  id @0 : UInt32;
  vertices @1 : List(MeshVertex);
}

struct NewMaterial {
  id @0 : UInt32;
}

struct NewDrawable {
  id @0 : UInt32;
  meshId @1 : UInt32;
  materialId @2 : UInt32;
}

struct DrawCall {
  mesh @0 : UInt32;
  material @1 : UInt32;
  x @2 : Float32;
  y @3 : Float32;
  scale @4 : Float32;
}

struct DrawList {
  list @0 : List(DrawCall);
}
