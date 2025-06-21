/*
 * tim.h
 *
 *  Created on: Aug 17, 2024
 *      Author: Nikolaos Grigoriadis
 *      Email : n.grigoriadis09@gmail.com
 *      Title : Embedded software engineer
 *      Degree: BSc and MSc in computer science, university of Ioannina
 */

#ifndef TIM_H_
#define TIM_H_

#include <main.h>

#define DEBOUNCE_TIME		0.3
#define SAMPLE_FREQUENCY	10
#define MAXIMUM			    (DEBOUNCE_TIME * SAMPLE_FREQUENCY)


extern unsigned int integrator;  /* Will range from 0 to the specified MAXIMUM */

void tim2_init(void);
void tim2_start_it(void);
void tim2_stop_it(void);

#endif /* TIM_H_ */
