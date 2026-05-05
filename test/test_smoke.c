#include "unity.h"



void setUp(void) {}
void tearDown(void) {}

void test_smoke_BasicArithmetic(void)
{
    TEST_ASSERT_EQUAL_INT(2, 1 + 1);
}

void test_smoke_TrueIsTrue(void)
{
    TEST_ASSERT_TRUE(1);
}

void test_smoke_StringsMatch(void)
{
    TEST_ASSERT_EQUAL_STRING("hello", "hello");
}
