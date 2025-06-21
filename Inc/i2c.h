/*
 * i2c.h
 *
 *  Created on: Sep 22, 2024
 *      Author: Nikolaos Grigoriadis
 *      Email : n.grigoriadis09@gmail.com
 *      Title : Embedded software engineer
 *      Degree: BSc and MSc in computer science, university of Ioannina
 */

#ifndef I2C_H_
#define I2C_H_

#include <main.h>
#include <stdio.h>
#include <stdlib.h>
#include <lcd204a.h>


void I2C1_init(void);
void I2C1_write_byte(uint8_t address, uint8_t data);
int I2C1_write(uint8_t dev_addr, uint8_t reg_addr, const uint8_t *data, uint8_t size);
void I2C1_read_byte(uint8_t address, uint8_t *data);
int I2C1_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t size);
void I2C1_scan_bus(void);
#endif /* I2C_H_ */
