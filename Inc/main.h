/*
 * main.h
 *
 *  Created on: Jul 20, 2024
 *      Author: Nikolaos Grigoriadis
 *		Email : n.grigoriadis09@gmail.com
 *		Title : Embedded software engineer
 * 	 	Degree: BSc and MSc in computer science, university of Ioannina
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <uart.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <defines.h>
#include <stm32l0xx.h>
#include <stm32l053xx.h>



/*Carriage return (\r)*/
#define RETURN                '\015'
/*NL line feed, new line (\n)*/
#define NEWLINE               '\012'
/*Define NULL*/
#define EMPTY                 '\000'
/*Peripheral for serial logging*/
#define STM_SERIAL            USART2
/*Peripheral for communication with ESP32*/
#define ESP_UART              USART1
/*Define the systems clock HSI 16MHz*/
#define SYSTEM_CORE_CLOCK     16000000
/*Buffer size of time and date buffers*/
#define DATE_TIME_SIZE_BUFF   20
/*Maximum code retries*/
#define MAXIMUM_RETRIES       4
/*Server's IP address*/
#define SERVER_IP             "192.168.1.21"
/*Server's port number*/
#define SERVER_PORT           5050





#endif /* MAIN_H_ */
