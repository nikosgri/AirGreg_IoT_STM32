/*
 * uart.c
 *
 *  Created on: Jun 8, 2024
 *      Author: Nikolaos Grigoriadis
 *		Email : n.grigoriadis09@gmail.com
 *		Title : Embedded software engineer
 * 	 	Degree: BSc and MSc in computer science, university of Ioannina
 */


#include "uart.h"
#include <timebase.h>

/*Local variables*/
char uart_receive_buffer[SIZE_OF_INCOMING_DATA] = {0};  // Used to store incoming USART data on RX line
volatile uint8_t uart_receive_index = 0;                // Used as indexer on incoming data buffer
volatile uint8_t message_complete = 0;                  // Indicates end of reception


/**
 * @function uart2_init
 *
 * @brief Initialize USART2 peripheral for printing data to serial monitor.
 */
void uart2_init(void)
{
	/*Variable declaration*/
	int usart_div = 0;

	/*Initialize GPIO pins for UART2*/
	USART2_gpio_init();

	/*Enable clock access to USART2 peripheral*/
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

	/*Define word length*/
	USART2->CR1 &= ~(USART_CR1_M0 | USART_CR1_M1);

	/*Set oversampling by 16*/
	USART2->CR1 &= ~USART_CR1_OVER8;

	/*Set the baudrate*/
	usart_div = SYSTEM_CLOCK / BAUDRATE;
	USART2->BRR = usart_div;

	/*Set one stop bit*/
	MODIFY_REG(USART2->CR2, USART_CR2_STOP, (0x00 << USART_CR2_STOP_Pos));

	/*Enable transmiter*/
	USART2->CR1 |= USART_CR1_TE;

	/*Enable peripheral*/
	USART2->CR1 |= USART_CR1_UE;
}

/**
 * @function uart1Init
 *
 * @brief Initialize UART1 peripheral for communication with ESP32 module
 * @note PA9(TX), PA10(RX) => AF4
 */
void uart1_init(void)
{
    /* Variable declaration */
    int usart_div = 0;

    /* Enable clock access to UART1 peripheral */
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN);

    /****** PIN CONFIGURATION ******/

    /* Enable clock access to GPIOA */
    SET_BIT(RCC->IOPENR, RCC_IOPENR_GPIOAEN);

    /* Set TX pin (PA9) as alternate function mode */
    GPIOA->MODER |= GPIO_MODER_MODE9_1;
    GPIOA->MODER &= ~GPIO_MODER_MODE9_0;

    /* Define Alternate function type for TX pin */
    MODIFY_REG(GPIOA->AFR[1], GPIO_AFRH_AFSEL9, (0x04 << GPIO_AFRH_AFSEL9_Pos));

    /* Set RX pin (PA10) as alternate function mode */
    GPIOA->MODER |= GPIO_MODER_MODE10_1;
    GPIOA->MODER &= ~GPIO_MODER_MODE10_0;

    /* Define Alternate function type for RX pin */
    MODIFY_REG(GPIOA->AFR[1], GPIO_AFRH_AFSEL10, (0x04 << GPIO_AFRH_AFSEL10_Pos));

    /****** PERIPHERAL CONFIGURATION ******/

    /* Define word length (8 bits) */
    USART1->CR1 &= ~(USART_CR1_M0 | USART_CR1_M1);

    /* Set oversampling by 16 */
    USART1->CR1 &= ~USART_CR1_OVER8;

    /* Set the baudrate */
    usart_div = SYSTEM_CLOCK / BAUDRATE;
    USART1->BRR = usart_div;

    /* Set one stop bit */
    MODIFY_REG(USART1->CR2, USART_CR2_STOP, (0x00 << USART_CR2_STOP_Pos));

    /* Enable transmitter */
    USART1->CR1 |= USART_CR1_TE;

    /* Enable receiver */
    USART1->CR1 |= USART_CR1_RE;

    /* Enable RX interrupt on UART1 peripheral */
    USART1->CR1 |= USART_CR1_RXNEIE;

    /* Enable peripheral */
    USART1->CR1 |= USART_CR1_UE;

    /* Enable UART interrupt in NVIC */
	NVIC_SetPriority(USART1_IRQn, 1);  // Optional priority level
	NVIC_EnableIRQ(USART1_IRQn);
}

/**
 * @function uart_transmit_byte
 *
 * @brief Transmit a single byte.
 * @param USARTx : Usart peripheral instance.
 * @param data   : A single character to transmit
 */
void uart_transmit_byte(USART_TypeDef *USARTx, uint8_t data)
{
    /* Wait until the transmit data register is empty */
    while (!(USARTx->ISR & USART_ISR_TXE)) {}

	/*Write data to TDR register*/
	USARTx->TDR = (data & 0xFF);

	/*Wait until the transmition is completed successfully*/
	while (!(USARTx->ISR & USART_ISR_TC)) {}
}

/**
 * @function uart1_transmit
 *
 * @brief Transmit message to ESP32 through USART1 peripheral.
 * @param msg : The string to be transmitted.
 * @retval The size of data that has been transmitted to throuth the peripheral.
 */
int uart1_transmit(char *data, int size)
{
    /* Variable declaration */
    int i = 0;

    /* Initiate transmit process */
    for (i = 0; i < size; i++)
    {
        /* Wait until the transmit data register is empty */
        while (!(USART1->ISR & USART_ISR_TXE)) {}

        /* Write data to TDR register */
        USART1->TDR = (data[i] & 0xFF);

        /* Wait until the transmission is completed successfully */
        while (!(USART1->ISR & USART_ISR_TC)) {}
    }

    /* Return the size of the transmitted data */
    return size;
}


/**
 * @brief Enable the UART1 receive data register not empty interrupt.
 *
 * This function enables the RXNEIE bit in USART1->CR1, which allows
 * the USART1 interrupt handler to be triggered when new data is received.
 *
 * @note Ensure that the NVIC interrupt for USART1 is already enabled
 *       with NVIC_EnableIRQ(USART1_IRQn) during peripheral initialization.
 */
void uart1_enable_rx_interrupt(void)
{
    USART1->CR1 |= USART_CR1_RXNEIE;

    /* Enable UART interrupt in NVIC */
	NVIC_SetPriority(USART1_IRQn, 1);  // Optional priority level
	NVIC_EnableIRQ(USART1_IRQn);
}


/**
 * @brief Disable the UART1 receive data register not empty interrupt.
 *
 * This function disables the RXNEIE bit in USART1->CR1 to stop the USART1
 * interrupt handler from being triggered when data is received.
 *
 * It also clears the RXNE flag (if set) by reading USART1->RDR, to prevent
 * unwanted interrupt triggering after disabling.
 *
 * @note Always clear RXNE before disabling the interrupt to avoid pending
 *       interrupts firing after RXNEIE is cleared.
 */
void uart1_disable_rx_interrupt(void)
{
   // Disable RXNE interrupt
   USART1->CR1 &= ~USART_CR1_RXNEIE;

   // Clear RXNE flag if set by reading RDR
   if (USART1->ISR & USART_ISR_RXNE)
   {
	   volatile uint8_t dummy = USART1->RDR;
	   (void)dummy; // Prevent unused variable warning
   }
}
