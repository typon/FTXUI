#include "ftxui/dom/flow_helper.hpp"
#include <iostream>
#include "ftxui/dom/box_helper.hpp"

namespace ftxui {
namespace flow_helper {

void SymmetryXY(FlowConfig& c) {
  switch (c.direction) {
    case FlowConfig::Direction::Row:
      c.direction = FlowConfig::Direction::Column;
      break;
    case FlowConfig::Direction::RowInversed:
      c.direction = FlowConfig::Direction::ColumnInversed;
      break;
    case FlowConfig::Direction::Column:
      c.direction = FlowConfig::Direction::Row;
      break;
    case FlowConfig::Direction::ColumnInversed:
      c.direction = FlowConfig::Direction::RowInversed;
      break;
  }
}

void SymmetryX(FlowConfig& c) {
  switch (c.direction) {
    case FlowConfig::Direction::Row:
      c.direction = FlowConfig::Direction::RowInversed; 
      break;
    case FlowConfig::Direction::RowInversed:
      c.direction = FlowConfig::Direction::Row;
      break;
    default:
      break;
  }
}

void SymmetryY(FlowConfig& c) {
  switch (c.wrap) {
    case FlowConfig::Wrap::NoWrap:
      break;
    case FlowConfig::Wrap::Wrap:
      c.wrap = FlowConfig::Wrap::WrapInversed;
      break;
    case FlowConfig::Wrap::WrapInversed:
      c.wrap = FlowConfig::Wrap::Wrap;
      break;
  }
}

void SymmetryXY(Global& g) {
  SymmetryXY(g.config);
  std::swap(g.size_x, g.size_y);
  for (auto& b : g.blocks) {
    std::swap(b.min_size_x, b.min_size_y);
    std::swap(b.flex_grow_x, b.flex_grow_y);
    std::swap(b.flex_shrink_x, b.flex_shrink_y);
    std::swap(b.x, b.y);
    std::swap(b.dim_x, b.dim_y);
  }
}

void SymmetryX(Global& g) {
  SymmetryX(g.config);
  for (auto& b : g.blocks) {
    b.x = g.size_x - b.x - b.dim_x;
  }
}

void SymmetryY(Global& g) {
  SymmetryY(g.config);
  for (auto& b : g.blocks) {
    b.y = g.size_y - b.y - b.dim_y;
  }
}

struct Line {
  std::vector<Block*> blocks;
};

void SetX(Global& global, std::vector<Line> lines) {
  for (auto& line : lines) {
    std::vector<box_helper::Element> elements;
    for (auto* block : line.blocks) {
      box_helper::Element element;
      element.min_size = block->min_size_x;
      element.flex_grow = block->flex_grow_x;
      element.flex_shrink = block->flex_shrink_x;
      elements.push_back(element);
    }

    box_helper::Compute(&elements, global.size_x);

    int x = 0;
    for (size_t i = 0; i < line.blocks.size(); ++i) {
      line.blocks[i]->dim_x = elements[i].size;
      line.blocks[i]->x = x;
      x += elements[i].size;
    }
  }
}

void SetY(Global& g, std::vector<Line> lines) {
  std::vector<box_helper::Element> elements;
  for (auto& line : lines) {
    box_helper::Element element;
    element.flex_shrink = line.blocks.front()->flex_shrink_y;
    element.flex_grow = line.blocks.front()->flex_grow_y;
    for (auto* block : line.blocks) {
      element.min_size = std::max(element.min_size, block->min_size_y);
      element.flex_shrink = std::min(element.flex_shrink, block->flex_shrink_y);
      element.flex_grow = std::min(element.flex_grow, block->flex_grow_y);
    }
    elements.push_back(element);
  }

  box_helper::Compute(&elements, g.size_y);

  int y = 0;
  for (size_t i = 0; i < lines.size(); ++i) {
    auto& element = elements[i];
    for (auto* block : lines[i].blocks) {
      block->y = y;
      block->dim_y = element.size;
    }
    y += element.size;
  }
}

void AlignContent(Global& g, std::vector<Line> lines) {
  (void)g;
  (void)lines;
}

void JustifyContent(Global& g, std::vector<Line> lines) {
  for (auto& line : lines) {
    Block* last = line.blocks.back();
    int remaining_space = g.size_x - last->x - last->dim_x;
    switch (g.config.justify_content) {
      case FlowConfig::JustifyContent::FlexStart: {
      } break;

      case FlowConfig::JustifyContent::FlexEnd: {
        for (auto* block : line.blocks)
          block->x += remaining_space;
      } break;

      case FlowConfig::JustifyContent::Center: {
        for (auto* block : line.blocks)
          block->x += remaining_space / 2;
      } break;

      case FlowConfig::JustifyContent::SpaceBetween: {
        for (int i = line.blocks.size() - 1; i >= 1; --i) {
          line.blocks[i]->x += remaining_space;
          remaining_space = remaining_space * (i - 1) / i;
        }
      } break;

      case FlowConfig::JustifyContent::SpaceAround: {
        for (int i = line.blocks.size() - 1; i >= 0; --i) {
          line.blocks[i]->x += remaining_space * (2 * i + 1) / (2 * i + 2);
          remaining_space = remaining_space * (2 * i) / (2 * i + 2);
        }
      } break;

      case FlowConfig::JustifyContent::SpaceEvenly: {
        for (int i = line.blocks.size() - 1; i >= 0; --i) {
          line.blocks[i]->x += remaining_space * (i + 1) / (i + 2);
          remaining_space = remaining_space * (i + 1) / (i + 2);
        }
      } break;
    }
  }
}

void Compute(Global& global) {
  if (global.config.direction == FlowConfig::Direction::Column ||
      global.config.direction == FlowConfig::Direction::ColumnInversed) {
    SymmetryXY(global);
    Compute(global);
    SymmetryXY(global);
    return;
  }

  if (global.config.direction == FlowConfig::Direction::RowInversed) {
    SymmetryX(global);
    Compute(global);
    SymmetryX(global);
    return;
  }

  if (global.config.wrap == FlowConfig::Wrap::WrapInversed) {
    SymmetryY(global);
    Compute(global);
    SymmetryY(global);
    return;
  }

  // Step 1: Lay out every elements into rows:
  std::vector<Line> lines;
  {
    Line line;
    int x = 0;
    for (auto& block : global.blocks) {
      // Does it fit the end of the row?
      // No? Then we need to start a new one:
      if (x + block.min_size_x > global.size_x) {
        x = 0;
        lines.push_back(std::move(line));
        line = Line();
      }

      block.line = lines.size();
      block.line_position = line.blocks.size();
      line.blocks.push_back(&block);
      x += block.min_size_x + global.config.gap_row;
    }
    lines.push_back(std::move(line));
  }

  // Step 2: Set positions on the X axis.
  SetX(global, lines);
  JustifyContent(global, lines);  // Distribute remaining space.

  // Step 3: Set positions on the Y axis.
  SetY(global, lines);
  AlignContent(global, lines);  // Distribute remaining space.
}

}  // namespace flow_helper
}  // namespace ftxui
