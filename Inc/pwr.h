/*
 * pwr.h
 *
 *  Created on: Aug 17, 2024
 *      Author: Nikolaos Grigoriadis
 *      Email : n.grigoriadis09@gmail.com
 *      Title : Embedded software engineer
 *      Degree: BSc and MSc in computer science, university of Ioannina
 */

#ifndef PWR_H_
#define PWR_H_

#include <main.h>

void enter_SleepMode();
void prepare_LowPower();
void mcu_WakeUp();
void Enter_Stop_Mode(void) ;

#endif /* PWR_H_ */
