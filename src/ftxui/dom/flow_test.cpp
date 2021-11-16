#include <gtest/gtest-message.h>  // for Message
#include <gtest/gtest-test-part.h>  // for SuiteApiResolver, TestFactoryImpl, TestPartResult
#include <algorithm>                // for remove
#include <string>                   // for allocator, basic_string, string
#include <vector>                   // for vector

#include "ftxui/dom/elements.hpp"  // for text, operator|, Element, flex, flex_grow, Elements, flex_shrink, vtext, gridbox, vbox, border
#include "ftxui/dom/node.hpp"      // for Render
#include "ftxui/screen/box.hpp"    // for ftxui
#include "ftxui/screen/screen.hpp"  // for Screen
#include "gtest/gtest_pred_impl.h"  // for Test, EXPECT_EQ, TEST

using namespace ftxui;

TEST(FlowTest, BasicRow) {
  auto root = flow(
      {
          text("aaa"),
          text("bbb"),
          text("cccc"),
          text("dddd"),
      },
      FlowConfig().Set(FlowConfig::Direction::Row));
  Screen screen(7, 4);
  Render(screen, root);
  EXPECT_EQ(screen.ToString(),
            "aaabbb \r\n"
            "cccc   \r\n"
            "dddd   \r\n"
            "       ");
}

TEST(FlowTest, BasicRowInversed) {
  auto root = flow(
      {
          text("aaa"),
          text("bbb"),
          text("cccc"),
          text("dddd"),
      },
      FlowConfig().Set(FlowConfig::Direction::RowInversed));
  Screen screen(7, 4);
  Render(screen, root);
  EXPECT_EQ(screen.ToString(),
            " bbbaaa\r\n"
            "   cccc\r\n"
            "   dddd\r\n"
            "       ");
}

TEST(FlowTest, BasicColumn) {
  auto root = flow(
      {
          text("aaa"),
          text("bbb"),
          text("cccc"),
          text("dddd"),
          text("e"),
      },
      FlowConfig().Set(FlowConfig::Direction::Column));

  Screen screen(8, 3);
  Render(screen, root);
  EXPECT_EQ(screen.ToString(),
            "aaa dddd\r\n"
            "bbb e   \r\n"
            "cccc    ");
}

TEST(FlowTest, BasicColumnInversed) {
  auto root = flow(
      {
          text("aaa"),
          text("bbb"),
          text("cccc"),
          text("dddd"),
          text("e"),
      },
      FlowConfig().Set(FlowConfig::Direction::ColumnInversed));

  Screen screen(8, 3);
  Render(screen, root);
  EXPECT_EQ(screen.ToString(),
            "cccc    \r\n"
            "bbb e   \r\n"
            "aaa dddd");
}

TEST(FlowTest, JustifyContentCenter) {
  auto root = flow(
      {
          text("aaa"),
          text("bbb"),
          text("cccc"),
          text("dddd"),
          text("e"),
      },
      FlowConfig().Set(FlowConfig::JustifyContent::Center));

  Screen screen(7, 4);
  Render(screen, root);
  EXPECT_EQ(screen.ToString(),
            "aaabbb \r\n"
            " cccc  \r\n"
            " dddde \r\n"
            "       ");
}

TEST(FlowTest, JustifyContentSpaceBetween) {
  auto root = flow(
      {
          text("aaa"),
          text("bbb"),
          text("cccc"),
          text("dddd"),
          text("e"),
      },
      FlowConfig().Set(FlowConfig::JustifyContent::SpaceBetween));

  Screen screen(7, 4);
  Render(screen, root);
  EXPECT_EQ(screen.ToString(),
            "aaa bbb\r\n"
            "cccc   \r\n"
            "dddd  e\r\n"
            "       ");
}

TEST(FlowTest, JustifyContentSpaceAround) {
  auto root = flow(
      {
          text("aa"),
          text("bb"),
          text("ccc"),
          text("dddddddddddd"),
          text("ee"),
          text("ff"),
          text("ggg"),
      },
      FlowConfig().Set(FlowConfig::JustifyContent::SpaceAround));

  Screen screen(15, 4);
  Render(screen, root);
  EXPECT_EQ(screen.ToString(),
            " aa  bb   ccc  \r\n"
            "ddddddddddddee \r\n"
            "  ff     ggg   \r\n"
            "               ");
}

TEST(FlowTest, JustifyContentSpaceEvenly) {
  auto root = flow(
      {
          text("aa"),
          text("bb"),
          text("ccc"),
          text("dddddddddddd"),
          text("ee"),
          text("ff"),
          text("ggg"),
      },
      FlowConfig().Set(FlowConfig::JustifyContent::SpaceAround));

  Screen screen(15, 4);
  Render(screen, root);
  EXPECT_EQ(screen.ToString(),
            " aa  bb   ccc  \r\n"
            "ddddddddddddee \r\n"
            "  ff     ggg   \r\n"
            "               ");
}

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
