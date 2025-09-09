/*
 * gpio.c
 *
 *  Created on: Jul 25, 2024
 *      Author: Nikolaos Grigoriadis
 *      Email : n.grigoriadis09@gmail.com
 *      Title : Embedded software engineer
 *      Degree: BSc and MSc in computer science, university of Ioannina
 */

#include <gpio.h>


void system_gpio_init(void)
{
	/** Connection button **/

	/* Enable clock access to GPIO port C */


}


/**
 * @brief Initialize GPIO pins for UART2 Peripheral
 * @note
 *    - PA2->TX->AF4.
 *    - PA3->RX->AF4
 */
void USART2_gpio_init(void)
{
	/*Enable clock access to GPIO port A*/
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;

	/*Set TX pin as alternate function mode*/
	GPIOA->MODER |= GPIO_MODER_MODE2_1;
	GPIOA->MODER &= ~GPIO_MODER_MODE2_0;

	/*Define Alternate function type*/
	MODIFY_REG(GPIOA->AFR[0], GPIO_AFRL_AFSEL2, (0x04 << GPIO_AFRL_AFSEL2_Pos));

	/*Set RX pin as alternate function mode*/
	GPIOA->MODER |= GPIO_MODER_MODE3_1;
	GPIOA->MODER &= ~GPIO_MODER_MODE3_0;

	/*Define alternate function type*/
	MODIFY_REG(GPIOA->AFR[0], GPIO_AFRL_AFSEL3, (0x04 << GPIO_AFRL_AFSEL3_Pos));
}


/**
 * @brief Initialize GPIO pins for I2C1 peripheral
 * @note
 *    - SCL->PB6->AF1
 *    - SDA->PB7->AF1
 */
void I2C1_gpio_init(void)
{
    /*Enable clock access to GPIO port B*/
    RCC->IOPENR |= RCC_IOPENR_GPIOBEN;

    /*Enable alternate function mode*/
    GPIOB->MODER |= (GPIO_MODER_MODE6_1 | GPIO_MODER_MODE7_1);
    GPIOB->MODER &= ~(GPIO_MODER_MODE6_0 | GPIO_MODER_MODE7_0);

    /*Set open drain mode*/
    GPIOB->OTYPER |= (GPIO_OTYPER_OT_7 | GPIO_OTYPER_OT_6);

    /*Set high speed to the each pin*/
    GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEED6_1 | GPIO_OSPEEDER_OSPEED7_1);
    GPIOB->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEED6_0 | GPIO_OSPEEDER_OSPEED7_0);

    /*Enable pull-up*/
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD6_1 | GPIO_PUPDR_PUPD7_1);
    GPIOB->PUPDR |= (GPIO_PUPDR_PUPD6_0 | GPIO_PUPDR_PUPD7_0);

    /*Select alternate function 1*/
    MODIFY_REG(GPIOB->AFR[0], GPIO_AFRL_AFSEL6, (0x01 << GPIO_AFRL_AFSEL6_Pos));
    MODIFY_REG(GPIOB->AFR[0], GPIO_AFRL_AFSEL7, (0x01 << GPIO_AFRL_AFSEL7_Pos));
}


/**
 * @brief Initialize GPIO pins for I2C2 peripheral
 * @note
 *    - SCL->PB13->AF5
 *    - SDA->PB14->AF5
 */
void I2C2_gpio_init(void)
{
	/*Enable clock access to GPIO port B*/
	RCC->IOPENR |= RCC_IOPENR_GPIOBEN;

	/*Set pins as alternate function mode*/
	GPIOB->MODER |= (GPIO_MODER_MODE13_1 | GPIO_MODER_MODE14_1);
	GPIOB->MODER &= ~(GPIO_MODER_MODE13_0 | GPIO_MODER_MODE14_0);

	/*Set both pin types to open drain*/
	GPIOB->OTYPER |= (GPIO_OTYPER_OT_13 | GPIO_OTYPER_OT_14);

	/*Enable high speed on both pins*/
	GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEED13_1 | GPIO_OSPEEDER_OSPEED14_1);
	GPIOB->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEED13_0 | GPIO_OSPEEDER_OSPEED14_0);

	/*Enable internal pull ups*/
	//TODO: Remember in final code to remove the pull ups since they will have hardware 10K externally
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD13_1 | GPIO_PUPDR_PUPD14_1);
	GPIOB->PUPDR |= (GPIO_PUPDR_PUPD13_0 | GPIO_PUPDR_PUPD14_1);

	/*Select alternate function 5*/
    MODIFY_REG(GPIOB->AFR[1], GPIO_AFRH_AFSEL13, (0x05 << GPIO_AFRH_AFSEL13_Pos));
    MODIFY_REG(GPIOB->AFR[1], GPIO_AFRH_AFSEL14, (0x05 << GPIO_AFRH_AFSEL14_Pos));
}



