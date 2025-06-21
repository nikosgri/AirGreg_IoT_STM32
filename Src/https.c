/*
 * https.c
 *
 *  Created on: May 24, 2025
 *      Author: grego
 */


#include "https.h"
#include "device.h"

static void strip_quotes(char *str);

https_status https_set_url(const char *url)
{
	/* Local variables */
	low_level_communication_state_t result = HTTPS_FAIL;
	uint16_t url_len = strlen(url);
	char command[20] = {0};

	if (url_len == 0)
	{
		LOG_WRN("Invalid URL");
		return HTTPS_FAIL;
	}

	/* Prepare command */
	snprintf(command, sizeof(command), "AT+HTTPURLCFG=%d", url_len);

	/* Send the length of the URL */
	//result = send_command_wait_result(command, ">", NULL, NULL, 0, 2000);
	result = send_command(command, NULL, NULL, ">", 0, 2000);
	if (result != LL_OK)
	{
		LOG_ERR("https_set_url() failed, with return code: %d", result);
		return result;
	}

	/* Enter the URL */
	result = send_command_wait_result(url, "SET OK", NULL, NULL, 0, 3000);
	if (result != LL_OK)
	{
		LOG_ERR("https_set_url() failed, with return code: %d", result);
		return result;
	}

	return HTTPS_OK;
}

https_status https_set_headers(const char *headers)
{
	/* Local variables */
	low_level_communication_state_t result = HTTPS_FAIL;
	uint16_t header_len = strlen(headers);
	char command[20] = {0};

	if (header_len == 0)
	{
		LOG_WRN("Invalid Headers");
		return HTTPS_FAIL;
	}

	/* Prepare command */
	snprintf(command, sizeof(command), "AT+HTTPCHEAD=%d", header_len);

	/* Send the length of the URL */
	result = send_command_wait_result(command, ">", NULL, NULL, 0, 4000);
	if (result != LL_OK)
	{
		LOG_ERR("https_set_headers() failed, with return code: %d", result);
		return result;
	}

	/* Enter the URL */
	result = send_command_wait_result(headers, "OK", NULL, NULL, 0, 5000);
	if (result != LL_OK)
	{
		LOG_ERR("https_set_headers() failed, with return code: %d", result);
		return result;
	}

	return HTTPS_OK;
}


https_status https_post(const char *url, const char *data)
{
	/* Local variables */
	low_level_communication_state_t result = HTTPS_FAIL;
	uint16_t data_len = strlen(data);
	char command[80] = {0};

	if (data_len == 0)
	{
		LOG_WRN("Invalid Data");
		return HTTPS_FAIL;
	}

	/* Prepare command */
	snprintf(command, sizeof(command), "AT+HTTPCPOST=\"%s\",%d", url, data_len);

	/* Send the length of the URL */
	result = send_command_wait_result(command, ">", NULL, NULL, 0, 2000);
	if (result != LL_OK)
	{
		LOG_ERR("https_post() failed, with return code: %d", result);
		return result;
	}

	/* Enter the URL */
	result = send_command_wait_result(data, "SEND OK", NULL, NULL, 0, 3000);
	if (result != LL_OK)
	{
		LOG_ERR("https_post() failed, with return code: %d", result);
		return result;
	}

	return HTTPS_OK;

}

https_status https_put(const char *url, const char *data)
{
	/* Local variables */
	low_level_communication_state_t result = HTTPS_FAIL;
	uint16_t data_len = strlen(data);
	char command[80] = {0};

	if (data_len == 0)
	{
		LOG_WRN("Invalid Data");
		return HTTPS_FAIL;
	}

	/* Prepare command */
	snprintf(command, sizeof(command), "AT+HTTPCPUT=\"%s\",%d", url, data_len);

	/* Send the length of the URL */
	result = send_command_wait_result(command, ">", NULL, NULL, 0, 2000);
	if (result != LL_OK)
	{
		LOG_ERR("https_put() failed, with return code: %d", result);
		return result;
	}

	/* Enter the URL */
	result = send_command_wait_result(data, "SEND OK", NULL, NULL, 0, 3000);
	if (result != LL_OK)
	{
		LOG_ERR("https_put() failed, with return code: %d", result);
		return result;
	}

	return HTTPS_OK;
}

void https_url_encode_colons(char *mac, char *encoded, size_t encoded_size)
{
    size_t i = 0;

    strip_quotes(mac);

    while (*mac && i + 3 < encoded_size) {
        if (*mac == ':') {
            // Replace ':' with "%3A"
            strncpy(&encoded[i], "%3A", 3);
            i += 3;
        } else {
            // Convert to uppercase and copy
            encoded[i++] = toupper((unsigned char)*mac);
        }
        mac++;
    }

    encoded[i] = '\0'; // Null-terminate
}

https_status https_update_firebase(void)
{
	/* Local variables */
	https_status result = HTTPS_FAIL;
	char send_buffer[150]={0};

	air_greg.air_quality=23.44;
	++air_greg.co2;
	air_greg.humidity =23.55;
	++air_greg.pm;
	air_greg.temperature=15.32;
	++air_greg.voc;

	/* Prepare the buffer */
	snprintf(send_buffer, sizeof(send_buffer), // @suppress("Float formatting support")
			"{\"temperature\":23.55,\"humidity\":25.23,\"voc\":%d,\"co2\":%d,\"air_quality\":12.56,\"pm\":%d}",
			 air_greg.voc, air_greg.co2, air_greg.pm);

	result = https_put("", send_buffer);

	return result;
}

static void strip_quotes(char *str) {
    size_t len = strlen(str);
    if (len >= 2 && str[0] == '"' && str[len - 1] == '"') {
        // Shift string left by 1 and terminate earlier to remove quotes
        memmove(str, str + 1, len - 2);
        str[len - 2] = '\0';
    }
}

