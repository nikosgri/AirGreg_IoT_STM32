/*
 * lcd2004.h
 *
 *  Created on: Sep 22, 2024
 *      Author: Nikolaos Grigoriadis
 *      Email : n.grigoriadis09@gmail.com
 *      Title : Embedded software engineer
 *      Degree: BSc and MSc in computer science, university of Ioannina
 */

#ifndef LCD204A_H_
#define LCD204A_H_

#include <main.h>
#include <i2c.h>
#include <timebase.h>

#define PCF8574_ADDRESS      0x4E
#define CLEAR_DISPLAY        0x01
#define RETURN_HOME          0x02


void LCD_init(void);
void LCD_send_command(uint32_t mode, int value);
void LCD_clear(void);
void LCD_return(void);

void LCD_print(char* str);

#endif
