/*
 * low_level_com.c
 *
 *  Created on: May 20, 2025
 *      Author: grego
 */


#include <low_level_com.h>



//low_level_communication_state_t send_command_wait_result(const char *command, const char *exp, const char *exp_parse,
//		const char *exp_end, uint32_t num_of_exp, uint32_t delay, ...)
//{
//    /* Variable declaration */
//    char command_to_send[strlen(command) + 3];    // Buffer to store the formatted command to send, including newline characters
//    char response_buffer[SIZE_OF_INCOMING_DATA];  // Buffer to store the response from ESP32
//    int response = -1;                 // Variable to hold the response status
//    bool command_waiting_to_be_send = true;
//    bool is_expression_parsed = false;
//    uint32_t start_time = get_tick();             // Stores the start time of the command execution
//
//    /* Clear buffers */
//    memset(response_buffer, 0, sizeof(response_buffer)); // Clear the response buffer
//    memset(uart_receive_buffer, 0, sizeof(uart_receive_buffer)); // Clear the UART receive buffer
//    memset(command_to_send, 0, sizeof(command_to_send)); // Clear the command buffer
//    uart_receive_index = 0; // Reset UART receive index
//
//#ifdef DEBUG_SYSTEM
//    if (command != NULL)
//    {
//        printf("%c>>>>", '\n'); // Start of debug output
//        if (command != NULL || command[0] != '\0') {
//        	printf("Command: %s%c%c", command, '\r', '\n'); // Print the command being sent
//        }
//    }
//#endif
//
//    while (response < 0)
//    {
//        /* Check for timeout */
//        if ((get_tick() - start_time) >= delay)
//        {
//#ifdef DEBUG_SYSTEM
//            LOG_WRN("Timeout occurred"); // Log a warning if a timeout occurs
//#endif
//            response = LL_TIMEOUT; // Set response status to timeout
//        }
//
//        /* Check if there is a command to be send */
//        if (command_waiting_to_be_send) {
//        	if (command != NULL) {
//        		/* Format and send the command */
//            	snprintf(command_to_send, sizeof(command_to_send), "%s\r\n", command); // Format the command with newline
//            	uart1_transmit(command_to_send, strlen(command_to_send)); // Transmit the command via UART
//            	command_waiting_to_be_send = 0;
//        	} else {
//        		continue; // There is no need of sending a command
//        	}
//        }
//
//        /* Check if the expression is found */
//        char *index_of_expr = strstr(uart_receive_buffer, exp);
//
//        if (index_of_expr != NULL) // Found the expected expression from sensor response.
//        {
//        	strncpy(response_buffer, uart_receive_buffer, sizeof(response_buffer) - 1); // Copy response to buffer
//
//        	if (exp_parse == NULL) // No need to parse something
//        	{
//				response = LL_OK;
//        	}
//        	else if (exp_parse != NULL) // Parse the expression based on user input
//        	{
//        		char *expr_start = strstr(uart_receive_buffer, exp);
//        		if (expr_start != NULL)
//        		{
//        			if (!is_expression_parsed)
//        			{
//        				expr_start += strlen(exp);  // Move past the expected string
//						va_list args;
//						va_start(args, delay);
//						/* Use vsscanf to read the variadic arguments */
//						int extracted_data = vsscanf(expr_start, exp_parse, args);
//						va_end(args);
//
//						if (extracted_data == num_of_exp)
//						{
//							is_expression_parsed = true;
//						}
//        			}
//        			else if (is_expression_parsed)
//        			{
//        				/* Check if there is an end expression that the user waits for */
//        				if (exp_end != NULL)
//        				{
//        					char *index_of_expr_end = strstr(uart_receive_buffer, exp_end);
//        					if (index_of_expr_end != NULL)
//        					{
//        						response = LL_OK;
//        					}
//        					continue;
//        				}
//        				else if (exp_end == NULL)
//        				{
//        					response = LL_OK;
//        				}
//        			}
//        		}
//        	}
//        }
//        else
//        {
//        	if (strstr(uart_receive_buffer, "ERROR"))
//        	{
//        		response = LL_ERROR;
//        	}
//        	else if (strstr(uart_receive_buffer, "SEND FAIL"))
//        	{
//        		/* Send command again */
//        		delay_ms(500);
//        		memset(uart_receive_buffer, 0, sizeof(uart_receive_buffer));
//        		command_waiting_to_be_send = true;
//        		continue;
//        	}
//        }
//    }
//
//#ifdef DEBUG_SYSTEM
//    printf("%s\r\n", response_buffer);
//    printf("<<<<");
//    printf("%c%c%c%c", RETURN, NEWLINE, RETURN, NEWLINE);
//#endif
//
//    return response; // Return the final response status
//}

low_level_communication_state_t send_command_wait_result(
    const char *command,
    const char *exp,
    const char *exp_parse,
    const char *exp_end,
    uint32_t num_of_exp,
    uint32_t delay,
    ...)
{
    char response_buffer[SIZE_OF_INCOMING_DATA] = {0};
    char command_to_send[strlen(command) + 3];
    int response = -1;
    uint32_t start_time = get_tick();

    // Clear UART buffer
    memset(uart_receive_buffer, 0, sizeof(uart_receive_buffer));
    uart_receive_index = 0;

    // Format and send command
    snprintf(command_to_send, sizeof(command_to_send), "%s\r\n", command);
    uart1_transmit(command_to_send, strlen(command_to_send));

#ifdef DEBUG_SYSTEM
    printf("\n>>>>Command: %s\r\n", command);
#endif

    while (response < 0) {
        if ((get_tick() - start_time) >= delay) {
#ifdef DEBUG_SYSTEM
            LOG_WRN("Timeout occurred");
#endif
            return LL_TIMEOUT;
        }

        if (strstr(uart_receive_buffer, "ERROR")) {
            return LL_ERROR;
        }

        if (exp && strstr(uart_receive_buffer, exp)) {
            if (exp_parse && num_of_exp > 0) {
                char *start = strstr(uart_receive_buffer, exp);
                if (start) {
                    start += strlen(exp);

                    va_list args;
                    va_start(args, delay);
                    int parsed = vsscanf(start, exp_parse, args);
                    va_end(args);

                    if (parsed == num_of_exp) {
                        if (!exp_end || strstr(uart_receive_buffer, exp_end)) {
                            response = LL_OK;
                        }
                    }
                }
            } else {
                if (!exp_end || strstr(uart_receive_buffer, exp_end)) {
                    response = LL_OK;
                }
            }
        } else if (!exp && exp_end && strstr(uart_receive_buffer, exp_end)) {
            response = LL_OK;
        }
        delay_ms(1);
    }

#ifdef DEBUG_SYSTEM
    printf("%s\r\n<<<<\r\n", uart_receive_buffer);
#endif

    return response;
}

