#include "gtest/gtest.h"

extern "C" {
#include "0.h"
}

TEST(Simple, Check) {
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
