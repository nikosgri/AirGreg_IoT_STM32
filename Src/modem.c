/*
 * modem.c
 *
 *  Created on: Jul 13, 2025
 *      Author: grego
 */

#include "modem.h"


/**
* @brief Open a UDP connection to a server.
*
* @param server_ip Server IP address.
* @param port_number Server port number.
* @retval MODEM_OK on success, MODEM_ERROR otherwise.
*/
modem_status_t modem_open_connection(const char * server_ip, int port_number)
{
    /*Local variable declaration*/
	modem_status_t result_code = MODEM_ERROR;
	char command[50] = {0};
    int status;


    /*Check the UDP connection status*/
    result_code = send_command("AT+CIPSTATUS", "STATUS:", "%d", "OK", 1, 2000, &status);

    /*Check the connection*/
    if (status != CONNECTED)
    {
        LOG_WRN("UDP connection has been lost, modem status %d\n", result_code);

        return MODEM_ERROR;
    }

	/*Start a UDP connection*/
	snprintf(command, sizeof(command), "AT+CIPSTART=\"UDP\",\"%s\",%d,%d", server_ip, port_number, port_number);
	result_code = send_command(command, "CONNECT", NULL, "OK", 0, 6000);
	if (result_code != MODEM_OK)
	{
		LOG_ERR("Could not open the UDP connection, modem status %d", result_code);

		return MODEM_ERROR;
	}

    return MODEM_OK;
}


/**
* @brief Close an active UDP connection.
* @retval MODEM_OK on success, MODEM_ERROR otherwise.
*/
modem_status_t modem_close_connection(void)
{
    /*Local variable declaration*/
	modem_status_t result_code = MODEM_ERROR;

    /*Start a UDP connection*/
    result_code = send_command("AT+CIPCLOSE", "CLOSED", NULL, "OK", 0, 2000);
    if (result_code != MODEM_OK)
    {
        LOG_ERR("Could not close the UDP connection");

        return MODEM_ERROR;
    }

    return MODEM_OK;
}


/**
* @brief Send JSON payloads to the UDP server.
*
* @retval MODEM_OK on success, or error codes on failure
*/
modem_status_t modem_send_udp(void)
{
	/*Local variable declaration*/
	modem_status_t result_code = MODEM_ERROR;
	int count_loops = 0;
	char command[50] = {0};

	count_loops = cb.size;

	LOG_VRB("%d payloads found!", count_loops);

	for (int i=0; i<count_loops; i++)
	{
		/*Extract each payload*/
		char payload[PAYLOAD_SIZE] = {0};
		CircularBuffer_Pop(&cb, payload);

		/*Send JSON data to the UDP server*/
		snprintf(command, sizeof(command), "AT+CIPSEND=%d", strlen(payload)+2);
		result_code = send_command(command, "OK", NULL, ">", 0, 3000);
		if (result_code == 0) {
			/*Proceed with the actual data*/
			result_code = send_command(payload, "SEND OK", NULL, "SEND OK", 0, 2000);
			if (result_code != MODEM_OK) {
				LOG_WRN("Failed to send the payload, it is consider as historical");

				CircularBuffer_Push(&cb, payload);
			}
			continue;
		} else if (result_code != MODEM_OK)
		{
			LOG_WRN("Failed to send the payload, it is consider as historical");
			CircularBuffer_Push(&cb, payload);
		}

	}


	return result_code;
}


/**
* @brief Receive data from the UDP socket.
*
* @param response Buffer to store received data.
* @retval MODEM_OK on success, or error codes on failure
*/
modem_status_t modem_receive_data(char * response)
{
    /*Local variable declaration*/
	modem_status_t result_code = MODEM_ERROR;
    int timeout = 2000;
    char command[50] = {0};
    char server_ip[20] = {0};
    int server_port = -1;
    int payload_len = 0;
    uint32_t start_time = get_tick();


    /*Obtain Socket Data Length*/
    while ((payload_len == 0) && ((get_tick() - start_time) < timeout)){
        result_code = send_command("AT+CIPRECVLEN?", "+CIPRECVLEN:", "%d", "OK", 1, 5000, &payload_len);
        if (result_code != MODEM_OK)
        {
            return result_code;
        }
    }

    if (payload_len == 0)
    	return -1;


    /*Obtain socket data*/
    snprintf(command, sizeof(command), "AT+CIPRECVDATA=%d", payload_len);
    result_code = send_command(command, "+CIPRECVDATA:", "%d,\"%[^\"]\",%d,%[^\r\n]", "OK", 4, 5000, &payload_len, server_ip, &server_port, response);
    if (result_code != MODEM_OK)
    {
        return result_code;
    }

    LOG_INF("Server IP: %s", server_ip);
    LOG_INF("Server Port: %d", server_port);
    LOG_INF("Response Length:%d", payload_len);
    LOG_INF("JSON: %s", response);

    return MODEM_OK;
}


/**
* @brief Put the modem into light-sleep mode.
*
* @retval MODEM_OK on success, or error codes on failure
*/
modem_status_t modem_power_down(void)
{
    /*Local variable declaration*/
	modem_status_t result_code = MODEM_ERROR;

    /*Set the device to light-sleep mode.*/
    result_code = send_command("AT+SLEEP=1", "OK", NULL, "OK", 0, 2000);

    if (result_code != 0)
    {
        LOG_ERR("Could not set the device to sleep mode");

        return result_code;
    }

    return result_code;
}


/**
* @brief Check if the modem is accessible.
*
* @retval MODEM_OK on success, or error codes on failure
*/
modem_status_t modem_is_accessible(void)
{
    /*Local variables*/
	modem_status_t result_code = MODEM_ERROR;

    /*Check if the WiFi extender is accessible*/
    result_code = send_command("AT", "OK", NULL, "OK", 0, 1000);

    /*Check the result code*/
    if (result_code != MODEM_OK)
    {
        return result_code;
    }

    return MODEM_OK;
}


/**
* @brief Retrieve IMEI (MAC-based ID) from the modem.
*
* @retval MODEM_OK on success, or error codes on failure
*/
modem_status_t modem_get_imei(void)
{
    /*Local variables*/
	modem_status_t result_code = MODEM_ERROR;
    char tmp_imei[20];
    int j = 0;  // Index for the result array

    /*Take the IMEI number of the WiFi modem*/
    result_code = send_command("AT+CIPAPMAC?", "+CIPAPMAC:", "%s", "OK", 1, 1000, tmp_imei);

    /*Check the result code*/
    if (result_code != MODEM_OK)
    {
        return result_code;
    }

    /* Iterate over the original string */
	for (int i = 0; i < strlen(tmp_imei); i++) {
		if (tmp_imei[i] != ':') {  // If the character is not a colon
			node.IMEI_num[j++] = tmp_imei[i];  // Copy it to the result string
		}
	}

	node.IMEI_num[j] = '\0';  // Null-terminate the result string

    return MODEM_OK;
}


/**
* @brief Retrieve RSSI from the current Wi-Fi connection.
*
* @retval MODEM_OK on success, or error codes on failure
*/
modem_status_t modem_get_rssi(void)
{
    /*Local variables*/
	modem_status_t result_code = MODEM_ERROR;
    char ssid[20]={0}, bssid[20]={0};
    int pci_n, channel, reconn_interval, listen_interval, scan_mode, pmf;

    /*Take the IMEI number of the WiFi modem*/
    result_code = send_command("AT+CWJAP?", "+CWJAP:", "\"%[^\"]\",\"%[^\"]\",%d,%d,%d,%d,%d,%d,%d", "OK", 9, 4000, ssid, bssid, &channel, &node.RSSI, &pci_n, &reconn_interval, &listen_interval, &scan_mode, &pmf);

    /*Check the result code*/
    if (result_code != MODEM_OK)
    {
        return result_code;
    }

    return MODEM_OK;
}


/**
 * @brief Retrieves the current modem sleep mode state.
 *
 * This function sends the "AT+SLEEP?" command to check the current modem sleep mode state.
 *
 * @retval returns the current sleep mode state on success, -1 otherwise
 */
int modem_get_sleep_state(void)
{
    /*Local variables*/
    int result_code = -1;
    int sleep_mode = -1;

    /*Check and return the sleep mode*/
    result_code = send_command("AT+SLEEP?", "+SLEEP:", "%d", "OK", 1, 2000, &sleep_mode);
    if (result_code != 0)
    {
        return result_code;
    }

    return sleep_mode;
}


/**
* @brief Send AT command and wait for response.
*
* @param command AT command string.
* @param exp Expected response substring.
* @param exp_parse Format to parse response.
* @param exp_end Expected end marker.
* @param num_of_exp Number of expected values.
* @param delay Timeout in ms.
* @param ... Variadic arguments for parsed values.
* @retval MODEM_OK on success, or error codes on failure
*/
modem_status_t send_command(const char *command, const char *exp, const char *exp_parse, const char *exp_end, uint32_t num_of_exp, uint32_t delay, ...)
{
    /* Variable declaration */
    char response_buffer[SIZE_OF_INCOMING_DATA];  // Buffer to store the response from ESP32
    char command_to_send[strlen(command) + 3];    // Buffer to store the formatted command to send, including newline characters
    int response = WIFI_OK - 100;                 // Variable to hold the response status
    uint32_t start_time = get_tick();             // Stores the start time of the command execution

    /* Clear buffers */
    memset(response_buffer, 0, sizeof(response_buffer)); // Clear the response buffer
    memset(uart_receive_buffer, 0, sizeof(uart_receive_buffer)); // Clear the UART receive buffer
    memset(command_to_send, 0, sizeof(command_to_send)); // Clear the command buffer
    uart_receive_index = 0; // Reset UART receive index

    /* Format and send the command */
    if (command != NULL || command[0] != '\0') {
    	snprintf(command_to_send, sizeof(command_to_send), "%s\r\n", command); // Format the command with newline
    	uart1_transmit(command_to_send, strlen(command_to_send)); // Transmit the command via UART
    }

    LOG_VRB(">>>>"); // Start of debug output
    if (command != NULL || command[0] != '\0') {
    	LOG_VRB("Command: %s%c%c", command, '\r', '\n'); // Print the command being sent
    }


    /* Wait for the response from the device */
    while (response < 0)
    {
        /* Check for timeout */
        if ((get_tick() - start_time) >= delay)
        {
            LOG_WRN("Timeout occurred"); // Log a warning if a timeout occurs
            response = MODEM_TIMEOUT; // Set response status to timeout

            break;
        }

        /* Check if the expected end of response is received */
        if (strstr(uart_receive_buffer, exp_end))
        {
            strncpy(response_buffer, uart_receive_buffer, sizeof(response_buffer) - 1); // Copy response to buffer
            response = MODEM_OK; // Set response status to success

            /* Parse the response data if needed */
            if (exp != NULL && exp_parse != NULL)
            {
                char *exp_start = strstr(response_buffer, exp); // Find the start of the expected data
                if (exp_start != NULL)
                {
                    exp_start += strlen(exp);  // Move past the expected string

                    va_list args;
                    va_start(args, delay);

                    /* Use vsscanf to read the variadic arguments */
                    vsscanf(exp_start, exp_parse, args);

                    va_end(args);
                }
            }

            break;
        }
    }

    /* Print the response if available */
    if (response_buffer[0] != '\0')
    {
    	LOG_VRB("%s\r\n", response_buffer); // Print the response buffer
    }

    LOG_VRB("<<<<");
    printf("%c%c%c%c", RETURN, NEWLINE, RETURN, NEWLINE);

    return response; // Return the final response status
}

