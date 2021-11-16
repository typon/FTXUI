#ifndef FTXUI_DOM_FLOW_CONFIG_HPP
#define FTXUI_DOM_FLOW_CONFIG_HPP

/*
  This replicate the CSS flexbox model.
  See guide for documentation:
  https://css-tricks.com/snippets/css/a-guide-to-flexbox/
*/

namespace ftxui {

struct FlowConfig {
  enum class Direction {
    Row,
    RowInversed,
    Column,
    ColumnInversed
  };
  Direction direction = Direction::Row;

  enum class Wrap {
    NoWrap,
    Wrap,
    WrapInversed,
  };
  Wrap wrap = Wrap::Wrap;

  enum class JustifyContent {
    FlexStart,
    FlexEnd,
    Center,
    SpaceBetween,
    SpaceAround,
    SpaceEvenly
  };
  JustifyContent justify_content = JustifyContent::FlexStart;

  enum class AlignItems {
    FlexStart,
    FlexEnd,
    Center,
  };
  AlignItems align_items = AlignItems::FlexStart;

  enum class AlignContent {
    FlexStart,
    FlexEnd,
    Center,
    Stretch,
    SpaceBetween,
    SpaceAround,
  };
  AlignContent align_content = AlignContent::FlexStart;

  int gap_row = 0;
  int gap_column = 0;

  // Constructor pattern. For chained use like:
  // ```
  // FlowConfig()
  //    .Set(FlowConfig::Direction::Row)
  //    .Set(FlowConfig::Wrap::Wrap);
  // ```
  FlowConfig& Set(FlowConfig::Direction);
  FlowConfig& Set(FlowConfig::Wrap);
  FlowConfig& Set(FlowConfig::JustifyContent);
  FlowConfig& Set(FlowConfig::AlignItems);
  FlowConfig& Set(FlowConfig::AlignContent);
  FlowConfig& SetGap(int gap_row, int gap_column);
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_DOM_FLOW_CONFIG_HPP */

// Copyright 2021 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
