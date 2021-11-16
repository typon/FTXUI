#include <algorithm>  // for max
#include <iostream>
#include <memory>   // for __shared_ptr_access, make_shared, shared_ptr
#include <utility>  // for move
#include <vector>   // for vector

#include "ftxui/dom/elements.hpp"  // for Element, Elements, hflow
#include "ftxui/dom/flow_helper.hpp"
#include "ftxui/dom/node.hpp"         // for Node
#include "ftxui/dom/requirement.hpp"  // for Requirement
#include "ftxui/screen/box.hpp"       // for Box

namespace ftxui {

void Normalize(FlowConfig::Direction& direction) {
  switch (direction) {
    case FlowConfig::Direction::Row:
    case FlowConfig::Direction::RowInversed: {
      direction = FlowConfig::Direction::Row;
    } break;
    case FlowConfig::Direction::Column:
    case FlowConfig::Direction::ColumnInversed: {
      direction = FlowConfig::Direction::Column;
    } break;
  }
}

void Normalize(FlowConfig::JustifyContent& justify) {
  justify = FlowConfig::JustifyContent::FlexStart;
}

void Normalize(FlowConfig::Wrap& wrap) {
  wrap = FlowConfig::Wrap::Wrap;
}

void Normalize(FlowConfig& config) {
  Normalize(config.direction);
  Normalize(config.wrap);
  Normalize(config.justify_content);
}

class Flow : public Node {
 public:
  Flow(Elements children, FlowConfig config)
      : Node(std::move(children)), config_(config) {
    requirement_.flex_grow_x = 1;
    requirement_.flex_grow_y = 0;

    if (IsColumnOriented())
      std::swap(requirement_.flex_grow_x, requirement_.flex_grow_y);
  }

  bool IsColumnOriented() {
    return config_.direction == FlowConfig::Direction::Column ||
           config_.direction == FlowConfig::Direction::ColumnInversed;
  }

  flow_helper::Global Layout(bool normalize) {
    flow_helper::Global global;
    global.config = config_;
    if (normalize)
      Normalize(global.config);
    global.size_x = asked_;
    global.size_y = 100000;
    if (IsColumnOriented())
      std::swap(global.size_x, global.size_y);
    for (auto& child : children_) {
      flow_helper::Block block;
      block.min_size_x = child->requirement().min_x;
      block.min_size_y = child->requirement().min_y;
      block.flex_grow_x = child->requirement().flex_grow_x;
      block.flex_grow_y = child->requirement().flex_grow_y;
      block.flex_shrink_x = child->requirement().flex_shrink_x;
      block.flex_shrink_y = child->requirement().flex_shrink_y;
      global.blocks.push_back(block);
    }

    flow_helper::Compute(global);
    return global;
  }

  void ComputeRequirement() override {
    for (auto& child : children_)
      child->ComputeRequirement();
    auto global = Layout(/*normalize=*/true);

    if (global.blocks.size() == 0) {
      requirement_.min_x = 0;
      requirement_.min_y = 0;
      return;
    }

    Box box;
    box.x_min = global.blocks[0].x;
    box.y_min = global.blocks[0].y;
    box.x_max = global.blocks[0].x + global.blocks[0].dim_x;
    box.y_max = global.blocks[0].y + global.blocks[0].dim_y;

    for (auto& b : global.blocks) {
      box.x_min = std::min(box.x_min, b.x);
      box.y_min = std::min(box.y_min, b.y);
      box.x_max = std::max(box.x_max, b.x + b.dim_x);
      box.y_max = std::max(box.y_max, b.y + b.dim_y);
    }

    requirement_.min_x = box.x_max - box.x_min;
    requirement_.min_y = box.y_max - box.y_min;
  }

  void SetBox(Box box) override {
    Node::SetBox(box);

    asked_ = std::min(asked_, IsColumnOriented() ? box.y_max - box.y_min + 1
                                                 : box.x_max - box.x_min + 1);
    auto global = Layout(/*normalize=*/false);

    need_iteration_ = false;
    for(size_t i = 0; i < children_.size(); ++i) {
      auto& child = children_[i];
      auto& b = global.blocks[i];

      Box children_box;
      children_box.x_min = box.x_min + b.x;
      children_box.y_min = box.y_min + b.y;
      children_box.x_max = box.x_min + b.x + b.dim_x - 1;
      children_box.y_max = box.y_min + b.y + b.dim_y - 1;

      Box intersection = Box::Intersection(children_box, box);
      child->SetBox(intersection);

      need_iteration_ |= (intersection != children_box);
    }
  }

  void Check(Status* status) override {
    for (auto& child : children_)
      child->Check(status);

    if (status->iteration == 0) {
      asked_ = 6000;
      need_iteration_ = true;
    }

    status->need_iteration |= need_iteration_;
  }

  int asked_ = 6000;
  bool need_iteration_ = true;
  FlowConfig config_;
};

/// @brief A container displaying elements on row/columns and capable of
/// wrapping on the next column/row when full.
/// @param children The elements in the container
/// @param config The option
/// @return The container.
///
/// #### Example
///
/// ```cpp
/// flow({
///   text("element 1"),
///   text("element 2"),
///   text("element 3"),
/// }, FlowConfig()
//       .Set(FlowConfig::Direction::Column)
//       .Set(FlowConfig::Wrap::WrapInversed)
//       .SetGapMainAxis(1)
//       .SetGapCrossAxis(1)
//  )
/// ```
Element flow(Elements children, FlowConfig config) {
  return std::make_shared<Flow>(std::move(children), std::move(config));
}

/// @brief A container displaying elements in rows from left to right. When
/// filled, it starts on a new row below.
/// @param children The elements in the container
/// @return The container.
///
/// #### Example
///
/// ```cpp
/// hflow({
///   text("element 1"),
///   text("element 2"),
///   text("element 3"),
/// });
/// ```
Element hflow(Elements children) {
  return flow(std::move(children), FlowConfig());
}

/// @brief A container displaying elements in rows from top to bottom. When
/// filled, it starts on a new columns on the right.
/// filled, it starts on a new row.
/// is full, it starts a new row.
/// @param children The elements in the container
/// @return The container.
///
/// #### Example
///
/// ```cpp
/// vflow({
///   text("element 1"),
///   text("element 2"),
///   text("element 3"),
/// });
/// ```
Element vflow(Elements children) {
  return flow(std::move(children),
              FlowConfig().Set(FlowConfig::Direction::Column));
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
