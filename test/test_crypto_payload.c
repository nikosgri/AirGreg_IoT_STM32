

#include "unity.h"

#include "crypto_payload.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_should_returnErrorWhenPayloadIsNULL(void)
{
	int result = crypto_payload_validate(NULL, 16);
	TEST_ASSERT_EQUAL_INT(CRYPTO_ERR_NULL, result);
}

void test_should_returnErrorIfSizeIsZero(void)
{
	int result = crypto_payload_validate("under_test_process", 0);
	TEST_ASSERT_EQUAL_INT(CRYPTO_ERR_INVALID_SIZE, result);
}


void test_should_returnOkIfSizeIsMultipleOf16(void)
{
	int result = crypto_payload_validate("under_test_process", 32);
	TEST_ASSERT_EQUAL_INT(CRYPTO_OK, result);
}

void test_should_ReturnErrorIfSizeIsLessThan16(void)
{
	int result = crypto_payload_validate("under_test_process", 8);
	TEST_ASSERT_EQUAL_INT(CRYPTO_ERR_INVALID_SIZE, result);
}

void test_should_ReturnErrorIfSizeIsTooLarge(void)
{
	int result = crypto_payload_validate("under_test_process", 1025);
	TEST_ASSERT_EQUAL_INT(CRYPTO_ERR_TOO_LARGE, result);
}

void test_should_ReturnErrorIfPayloadIsNotAligned(void)
{
    uint8_t buffer[20];

    crypto_status_t status = crypto_payload_validate(&buffer[2], 16);

    TEST_ASSERT_EQUAL(CRYPTO_ERR_ALIGNMENT, status);
}
