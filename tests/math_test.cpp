#include "gtest/gtest.h"


TEST(SimpleMathTest, Sum) {
	// TEST(TestCase, TestName
	EXPECT_EQ(2, 1 + 1);
	EXPECT_EQ(4, 2 + 2);
	// EXPECT_EQ(expected, actual)
}

TEST(SimpleMathTest, Difference) {
	EXPECT_EQ(0, 1 - 1);
	EXPECT_EQ(1, 2 - 1);
	EXPECT_EQ(2, 3 - 1);
}

TEST(SimpleMathTest, Multiplication) {
	EXPECT_EQ(1, 1 * 1);
}