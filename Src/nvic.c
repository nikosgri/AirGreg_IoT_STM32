/*
 * nvic.c
 *
 *  Created on: Jul 22, 2024
 *      Author: Nikolaos Grigoriadis
 *      Email : n.grigoriadis09@gmail.com
 *      Title : Embedded software engineer
 *      Degree: BSc and MSc in computer science, university of Ioannina
 */


#include <main.h>
#include <uart.h>
#include <swo.h>
#include <timebase.h>
#include <pwr.h>
#include <device.h>
#include <tim.h>

/**
 * @brief Receives responses from ESP32 module.
 */
void USART1_IRQHandler(void)
{
    /* Read RXNE bit to check if incoming data has arrived to RX_BUFFER */
    if (READ_BIT(USART1->ISR, USART_ISR_RXNE))
    {
        /* Read RDR register to retrieve data */
        uart_receive_buffer[uart_receive_index] = (USART1->RDR & 0xFF);

        /* Update circular buffer index */
        uart_receive_index = (uart_receive_index + 1) % SIZE_OF_INCOMING_DATA;
    }
}

/**
 * @brief Responsible for waking up the MCU from low power when the RTC AlarmA triggers.
 */
void RTC_IRQHandler(void)
{
    if (RTC->ISR & RTC_ISR_ALRAF)
    {
        /*Clear the Alarm A flag*/
        RTC->ISR &= ~RTC_ISR_ALRAF;

        /*Clear EXTI line 17 pending flag*/
        EXTI->PR |= EXTI_PR_PR17;

        /*Add code here to handle MCU wakeup from Stop mode, if needed*/
        mcu_WakeUp();

        /*Enable keep alive message*/
        device.flg.keep_alive = 1;
    }
}

void EXTI4_15_IRQHandler(void)
{
	/*Check if PC13 (Pulse1) pin has triggered the interrupt*/
	if ((EXTI->PR & EXTI_PR_PIF13) == EXTI_PR_PIF13)
	{
        /* Clear EXTI13 interrupt flag */
        EXTI->PR = EXTI_PR_PIF13;

        /*Start timer for switch debouncing control*/
        tim2_start_it();
	}
}

void TIM2_IRQHandler(void)
{
	if ((TIM2->SR & TIM_SR_UIF) == TIM_SR_UIF)
	{
		unsigned int input_signal; // Hold the state of the pin.

		if ((GPIOC->IDR & GPIO_IDR_ID13) == GPIO_IDR_ID13)
		{
			/*High state*/
			input_signal = 1;
		}
		else
		{
			/*Low state*/
			input_signal = 0;
		}

		/*ebounce logic - update integrator */
		if (input_signal == 0)
		{
			if (integrator > 0)
			{
				integrator--;
			}
		}
		else if (integrator < MAXIMUM)
		{
			integrator++;
		}

		/*If integrator reaches the threshold, update pulse count */
		if (integrator >= MAXIMUM)
		{
			/*Valid pulse detected*/
			pulse1_counter++;

			device.flg.ble_conn =1;

			/*Ensure it does not exceed MAXIMUM*/
			integrator = MAXIMUM;

			/* Stop TIM2 after first valid pulse */
			tim2_stop_it();
		}
	}
}

/**
 * @brief Hard Fault interrupt handler.
 *
 * This function is called when a Hard Fault exception occurs.
 * It enters an infinite loop to halt the system. This is a basic
 * implementation for error handling, and in a real application,
 * you might want to add more error reporting or recovery code.
 *
 * @retval None
 */
void HardFault_Handler(void)
{
    while (1)
    {
        // Infinite loop
    	LOG_ERR("Stack on while\r\n");
    	for(int i=0;i<10000000;i++);
    }
}

/**
 * @brief SysTick interrupt handler.
 *
 * This function is called when the SysTick timer generates an interrupt.
 * It increments the system tick count, which is typically used for
 * timekeeping and delays in the application.
 *
 * @retval None
 */
void SysTick_Handler(void)
{
    /*Increase current tick events*/
    tick_increment();
}

