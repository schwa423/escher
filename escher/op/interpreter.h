// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace escher {

struct Op {
  size_t type;
  std::vector<uint8_t> data;
};

class Interpreter {
 public:
  bool Do(Op* op);

  typedef std::function<bool(Op*)> Action;
  class Table {
   public:
    struct Entry {
      std::string name;
      Action action;
    };

    explicit Table(uint32_t offset = 0, std::vector<Entry> entries = {});
    const Entry* GetEntry(uint32_t index) const;

    void AddEntry(Entry entry);

   private:
    uint32_t offset_;
    std::vector<Entry> entries_;
  };

  const Table& GetCurrentTable() const;

 protected:
  void PushTable(Table* table);
  void PopTable();

 private:
  Table* current_table_ = nullptr;
  std::vector<Table*> table_stack_;
};

}  // namespace escher
