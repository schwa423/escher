// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "escher/op/interpreter.h"

#include "ftl/logging.h"

namespace escher {

bool Interpreter::Do(Op* op) {
  auto entry = current_table_ ? current_table_->GetEntry(op->type) : nullptr;
  if (entry && entry->action(op)) {
    return true;
  } else {
    current_table_ = nullptr;
    table_stack_.clear();
    return false;
  }
}

const Interpreter::Table& Interpreter::GetCurrentTable() const {
  FTL_CHECK(current_table_);
  return *current_table_;
}

void Interpreter::PushTable(Table* table) {
  FTL_CHECK(table && current_table_ != table);
  current_table_ = table;
  table_stack_.push_back(table);
}

void Interpreter::PopTable() {
  FTL_CHECK(!table_stack_.empty());
  table_stack_.pop_back();
  current_table_ = table_stack_.empty() ? nullptr : table_stack_.front();
}

Interpreter::Table::Table(uint32_t offset, std::vector<Entry> entries)
    : offset_(offset), entries_(std::move(entries)) {}

const Interpreter::Table::Entry* Interpreter::Table::GetEntry(
    uint32_t index) const {
  FTL_CHECK(index - offset_ < entries_.size());
  return &entries_[index - offset_];
}

void Interpreter::Table::AddEntry(Entry entry) {
  entries_.push_back(std::move(entry));
}

}  // namespace escher
