/*
 * gpio.h
 *
 *  Created on: Jul 25, 2024
 *      Author: Nikolaos Grigoriadis
 *      Email : n.grigoriadis09@gmail.com
 *      Title : Embedded software engineer
 *      Degree: BSc and MSc in computer science, university of Ioannina
 */

#ifndef GPIO_H_
#define GPIO_H_

#include <main.h>

void system_gpio_init(void);
void USART2_gpio_init(void);
void I2C1_gpio_init(void);
void I2C2_gpio_init(void);

#endif /* GPIO_H_ */
