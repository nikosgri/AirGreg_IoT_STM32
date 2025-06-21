/*
 * uart.h
 *
 *  Created on: Jun 8, 2024
 *      Author: Nikolaos Grigoriadis
 *		Email : n.grigoriadis09@gmail.com
 *		Title : Embedded software engineer
 * 	 	Degree: BSc and MSc in computer science, university of Ioannina
 */

#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include <main.h>
#include <stm32l0xx.h>
#include <stm32l053xx.h>


/*System core clock*/
#define SYSTEM_CLOCK                            16000000
/*Desired baudrate*/
#define BAUDRATE                                115200
/*Define the size of incoming data buffer*/
#define SIZE_OF_INCOMING_DATA                   2048

/*USART function declaration*/
void uart1_init();
void uart2_init(void);
int uart1_transmit(char *data, int size);
void uart_transmit_byte(USART_TypeDef *USARTx, uint8_t data);
void uart1_enable_rx_interrupt(void);
void uart1_disable_rx_interrupt(void);

/*USART global variable declaration*/
extern char uart_receive_buffer[SIZE_OF_INCOMING_DATA];
extern volatile uint8_t uart_receive_index;
extern volatile uint8_t message_complete;


#endif /* UART_H_ */
