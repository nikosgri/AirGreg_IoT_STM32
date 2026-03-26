/*
 * test_main.cpp
 *
 *  Created on: Mar 11, 2026
 *      Author: grego
 */




/* Testing/test_main.cpp */
#include "gtest/gtest.h"
#include "TrivialClass.hpp"

int main(int ac, char* av[])
{
  testing::InitGoogleTest(&ac, av);
  return RUN_ALL_TESTS();
}

// Logic copied from wifi.c [_extract_month]
int test_extract_month(const char *month) {
    const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    for (int i = 0; i < 12; i++) {
        if (strcmp(months[i], month) == 0) return i;
    }
    return -1;
}

TEST(MonthLogic, HandlesValidMonths) {
    EXPECT_EQ(test_extract_month("Jan"), 0);
    EXPECT_EQ(test_extract_month("Dec"), 11);
    EXPECT_EQ(test_extract_month("Jul"), 6);
}

TEST(MonthLogic, HandlesInvalidInput) {
    EXPECT_EQ(test_extract_month("NotAMonth"), -1);
    EXPECT_EQ(test_extract_month("JAN"), -1); // Case sensitivity check
}
