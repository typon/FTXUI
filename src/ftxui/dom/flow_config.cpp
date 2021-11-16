#include "ftxui/dom/flow_config.hpp"

namespace ftxui {

FlowConfig& FlowConfig::Set(FlowConfig::Direction d) {
  this->direction = d;
  return *this;
}

FlowConfig& FlowConfig::Set(FlowConfig::Wrap w) {
  this->wrap = w;
  return *this;
}

FlowConfig& FlowConfig::Set(FlowConfig::JustifyContent j) {
  this->justify_content = j;
  return *this;
}

FlowConfig& FlowConfig::Set(FlowConfig::AlignItems a) {
  this->align_items = a;
  return *this;
}

FlowConfig& FlowConfig::Set(FlowConfig::AlignContent a) {
  this->align_content = a;
  return *this;
}

FlowConfig& FlowConfig::SetGap(int r, int c) {
  this->gap_row = r;
  this->gap_column = c;
  return *this;
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
