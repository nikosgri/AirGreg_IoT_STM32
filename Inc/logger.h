/*
 * swo.h
 *
 *  Created on: Jul 20, 2024
 *      Author: Nikolaos Grigoriadis
 *		Email : n.grigoriadis09@gmail.com
 *		Title : Embedded software engineer
 * 	 	Degree: BSc and MSc in computer science, university of Ioannina
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <main.h>
#include <stdarg.h>
#include <defines.h>

typedef enum {
	LOG_LEVEL_NONE,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_WARNING,
	LOG_LEVEL_INFO,
	LOG_LEVEL_VERBOSE
}log_level_t;


void LOG_INF(const char *msg, ...) ;
void LOG_ERR(const char *msg, ...) ;
void LOG_WRN(const char *msg, ...) ;
void LOG_VRB(const char *msg, ...) ;

#endif /* LOGGER_H_ */
