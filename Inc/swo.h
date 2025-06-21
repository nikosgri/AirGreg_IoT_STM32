/*
 * swo.h
 *
 *  Created on: Jul 20, 2024
 *      Author: Nikolaos Grigoriadis
 *		Email : n.grigoriadis09@gmail.com
 *		Title : Embedded software engineer
 * 	 	Degree: BSc and MSc in computer science, university of Ioannina
 */

#ifndef SWO_H_
#define SWO_H_

#include <main.h>
#include <stdarg.h>


void LOG_INF(const char *msg, ...) ;
void LOG_ERR(const char *msg, ...) ;
void LOG_WRN(const char *msg, ...) ;

#endif /* SWO_H_ */
