#include <iostream>
#include <algorithm>  // for max
#include <memory>     // for __shared_ptr_access, make_shared, shared_ptr
#include <utility>    // for move
#include <vector>     // for vector

#include "ftxui/dom/elements.hpp"     // for Element, Elements, hflow
#include "ftxui/dom/node.hpp"         // for Node
#include "ftxui/dom/requirement.hpp"  // for Requirement
#include "ftxui/screen/box.hpp"       // for Box

namespace ftxui {

class HFlow : public Node {
 public:
  HFlow(Elements children) : Node(std::move(children)) {}

  void ComputeRequirement() override {
    for (auto& child : children_)
      child->ComputeRequirement();

    // The position of the first component.
    int x = 0;
    int y = 0;
    int y_next = y;  // The position of next row of elements.

    for (auto& child : children_) {
      Requirement requirement = child->requirement();

      // Does it fit the end of the row?
      if (x + requirement.min_x > dimx_to_ask_) {
        // No? Use the next row.
        x = 0;
        y = y_next;
      }

      x = x + requirement.min_x;
      y_next = std::max(y_next, y + requirement.min_y);
    }

    requirement_.min_x = dimx_to_ask_;
    requirement_.min_y = y_next;
    requirement_.flex_grow_x = 1;
    requirement_.flex_grow_y = 0;
    requirement_.flex_shrink_x = 0;
    requirement_.flex_shrink_y = 0;
  }

  void SetBox(Box box) override {
    Node::SetBox(box);

    // The position of the first component.
    int x = box.x_min;
    int y = box.y_min;
    int y_next = y;  // The position of next row of elements.

    //std::cout << "asked: " << requirement_.min_x << "," << requirement_.min_y
              //<< std::endl;
    //std::cout << "given: " << box.x_max - box.x_min - 1 << ","
              //<< box.y_max - box.y_min - 1 << std::endl;
    int given =  box.x_max - box.x_min - 1;

    for (auto& child : children_) {
      Requirement requirement = child->requirement();

      // Does it fit the end of the row?
      if (x + requirement.min_x > box.x_max) {
        // No? Use the next row.
        x = box.x_min;
        y = y_next;
      }

      // Does the current row big enough to contain the element?
      if (y + requirement.min_y > box.y_max + 1) {
        dimx_to_ask_ = std::min(dimx_to_ask_, given);
    //std::cout << "need_iteration = 1" << std::endl;
        need_iteration_ = true;
        return;  // No? Ignore the element.
      }

      Box children_box;
      children_box.x_min = x;
      children_box.y_min = y;
      children_box.x_max = x + requirement.min_x - 1;
      children_box.y_max = y + requirement.min_y - 1;
      child->SetBox(children_box);

      x = x + requirement.min_x;
      y_next = std::max(y_next, y + requirement.min_y);
    }

    if (dimx_to_ask_ > given) {
      dimx_to_ask_ = given;
    //std::cout << "need_iteration = 1" << std::endl;
      need_iteration_ = true;
      return;
    }
    //std::cout << "need_iteration = 0" << std::endl;
    need_iteration_ = false;
  }

  void Check(Status* status) override {
    for (auto& child : children_)
      child->Check(status);

    if (status->iteration == 0) {
      dimx_to_ask_ = 2500;
      need_iteration_ = true;
    }

    status->need_iteration |= need_iteration_;
  }

  int dimx_to_ask_ = 2500;
  bool need_iteration_ = true;
};

/// @brief A container displaying elements horizontally one by one.
/// @param children The elements in the container
/// @return The container.
///
/// #### Example
///
/// ```cpp
/// hbox({
///   text("Left"),
///   text("Right"),
/// });
/// ```
Element hflow(Elements children) {
  return std::make_shared<HFlow>(std::move(children));
}

}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
