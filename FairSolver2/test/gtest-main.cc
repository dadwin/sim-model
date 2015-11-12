/*
 * gtest-main.cc
 *
 *  Created on: Oct 4, 2015
 *      Author: aarlan
 */



// gtest_main.cpp
#include <stdio.h>
#include <string>
#include "gtest/gtest.h"

size_t ResourceSize = 100;
size_t FlowSize = 100;

GTEST_API_ int main(int argc, char **argv) {
  printf("Running main() from gtest_main.cc\n");

  if (argc > 3) {
      ResourceSize = std::stol(argv[1]);
      FlowSize = std::stol(argv[2]);
  }

  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
