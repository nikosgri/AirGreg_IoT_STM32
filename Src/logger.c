/*
 * swo.c
 *
 *  Created on: Jul 20, 2024
 *      Author: Nikolaos Grigoriadis
 *		Email : n.grigoriadis09@gmail.com
 *		Title : Embedded software engineer
 * 	 	Degree: BSc and MSc in computer science, university of Ioannina
 */


#include <logger.h>


log_level_t log_level = LOG_LEVEL_INFO; // Defining log level


/**
 * @brief Retarget printf
 */
int __io_putchar(int ch)
{
	uart_transmit_byte(STM_SERIAL, ch);
	return ch;
}

static void log_message(const char *level, const char *format, log_level_t logger_level, va_list args);


void LOG_INF(const char *msg, ...) {
    va_list args;
    va_start(args, msg);
    log_message("(I)", msg, LOG_LEVEL_INFO, args);
    va_end(args);
}

void LOG_ERR(const char *msg, ...) {
    va_list args;
    va_start(args, msg);
    log_message("(E)", msg, LOG_LEVEL_ERROR, args);
    va_end(args);
}

void LOG_WRN(const char *msg, ...) {
    va_list args;
    va_start(args, msg);
    log_message("(W)", msg, LOG_LEVEL_WARNING, args);
    va_end(args);
}

void LOG_VRB(const char *msg, ...) {
    va_list args;
    va_start(args, msg);
    log_message("(V)", msg, LOG_LEVEL_VERBOSE, args);
    va_end(args);
}

static void log_message(const char *level, const char *format, log_level_t logger_level, va_list args)
{
	if ((log_level > logger_level) || logger_level == LOG_LEVEL_NONE) {
		return;
	}

    char buffer[128] = {0};

    // Format the string properly
    vsnprintf(buffer, sizeof(buffer), format, args);

    char output[140];  // Prefix + message
    snprintf(output, sizeof(output), "%s %s\n", level, buffer);

    for (int i=0; i<strlen(output); i++) {
    	uart_transmit_byte(STM_SERIAL, output[i]);
    }
}
