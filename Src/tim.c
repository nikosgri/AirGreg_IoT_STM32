/*
 * tim.c
 *
 *  Created on: Aug 17, 2024
 *      Author: Nikolaos Grigoriadis
 *      Email : n.grigoriadis09@gmail.com
 *      Title : Embedded software engineer
 *      Degree: BSc and MSc in computer science, university of Ioannina
 */


#include <tim.h>


unsigned int integrator;  /* Will range from 0 to the specified MAXIMUM */

/**
 * @brief Initializes timer 2 as 1MHz up counter with interrupt capability.
 */
void tim2_init(void)
{
    /* Enable clock access to TIM2 */
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    /* Ensure TIM2 is disabled before configuring */
    TIM2->CR1 &= ~TIM_CR1_CEN;

    /* Set the counter to count up */
    TIM2->CR1 &= ~TIM_CR1_DIR;

    /* Set the prescaler and auto-reload for a 1Hz interrupt */
    TIM2->PSC = 16000 - 1;  // Prescaler for 1 ms tick (assuming 16MHz system clock)
    TIM2->ARR = 10 - 1;   // Auto-reload for 10ms

    /* Reset the counter */
    TIM2->CNT = 0;

    /* Enable update interrupt */
    TIM2->DIER |= TIM_DIER_UIE;

    /* Enable TIM2 interrupt in NVIC */
    NVIC_EnableIRQ(TIM2_IRQn);
    NVIC_SetPriority(TIM2_IRQn, 1);
}


/**
 * @brief Starts timer interrupt.
 */
void tim2_start_it(void)
{
	/*Reset the counter*/
    TIM2->CNT = 0;

    /*Enable update interrupt*/
    TIM2->DIER |= TIM_DIER_UIE;

    /*Start timer*/
    TIM2->CR1 |= TIM_CR1_CEN;
}

/**
 * @brief Stops timer interrupt.
 */
void tim2_stop_it(void)
{
    /* Disable counter */
    TIM2->CR1 &= ~TIM_CR1_CEN;

    /* Disable update interrupt */
    TIM2->DIER &= ~TIM_DIER_UIE;
}

