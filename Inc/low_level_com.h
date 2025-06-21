/*
 * low_level_com.h
 *
 *  Created on: Jul 25, 2024
 *      Author: Nikolaos Grigoriadis
 *      Email : n.grigoriadis09@gmail.com
 *      Title : Embedded software engineer
 *      Degree: BSc and MSc in computer science, university of Ioannina
 */


#ifndef LOW_LEVEL_COM_H_
#define LOW_LEVEL_COM_H_

#include <main.h>
#include <uart.h>
#include <swo.h>
#include <stdarg.h>
#include <timebase.h>
#include <stdbool.h>

typedef enum {
	LL_OK,
	LL_BUSY,
	LL_TIMEOUT,
	LL_ERROR
}low_level_communication_state_t;

low_level_communication_state_t send_command_wait_result(const char *command, const char *exp, const char *exp_parse,
		const char *exp_end, uint32_t num_of_exp, uint32_t delay, ...);


#endif /* LOW_LEVEL_COM_H_ */
