
#ifndef CRYPTO_PAYLOAD_H
#define CRYPTO_PAYLOAD_H


#include <stdint.h>
#include <stddef.h>



typedef enum {
	CRYPTO_OK = 0, //Success
	CRYPTO_ERR_NULL = -1, //Payload should not be null
	CRYPTO_ERR_INVALID_SIZE = -2, //Size should be multiple of 16, and not less than 16
	CRYPTO_ERR_ALIGNMENT = -3 //Cortex-M0+ requires 4-byte alignment
}crypto_status_t;

crypto_status_t crypto_payload_validate(const uint8_t *payload, uint16_t size);


#endif // CRYPTO_PAYLOAD_H
