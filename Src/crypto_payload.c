#include "crypto_payload.h"



crypto_status_t crypto_payload_validate(const uint8_t *payload, uint16_t size)
{

	if (payload == NULL) {
		return CRYPTO_ERR_NULL;
	} else if (((uintptr_t)payload & 0x03) != 0) {
		return CRYPTO_ERR_ALIGNMENT; // Cortex-M0+ requires 4-byte alignment for 32-bit DMA transfers to AES peripheral - hardfault otherwise
	}

	if (((size % 16) == 0) && size < 1024
			&& size > 0){
		return CRYPTO_OK;
	} else {

		if (size >= 1024) {
			return CRYPTO_ERR_TOO_LARGE;
		}

		return CRYPTO_ERR_INVALID_SIZE;
	}

	return CRYPTO_OK;
}


