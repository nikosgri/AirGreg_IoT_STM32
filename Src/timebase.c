/*
 * timebase.c
 *
 *  Created on: Jul 27, 2024
 *      Author: Nikolaos Grigoriadis
 *      Email : n.grigoriadis09@gmail.com
 *      Title : Embedded software engineer
 *      Degree: BSc and MSc in computer science, university of Ioannina
 */


#include <timebase.h>


/*Global variable*/
uint32_t current_tick = 0;

/**
 * @brief Enable systick timer with interrupt mode.
 * @param load_val: The starting counting value, which eventually goes down to zero.
 * @retval None.
 */
void systick_init(uint32_t load_val)
{
	/*Disable global interrupts*/
	__disable_irq();

    /*Programming the reload value*/
	WRITE_REG(SysTick->LOAD, (load_val - 1));

	/*Clear the current value*/
	WRITE_REG(SysTick->VAL, 0x00);

	/*Enable internal clock source*/
	SET_BIT(SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk);

	/*Enable SysTick exception request*/
	SET_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk);

	/*Enable the counter*/
	SET_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);

	/*Enable global interrupts*/
	__enable_irq();
}

/**
 * @brief This function increases current system ticks.
 * @retval None.
 */
void tick_increment()
{
    current_tick += 1;
}

/**
 * @brief returns Current tick event.
 * @retval current tick.
 */
uint32_t get_tick()
{
    uint32_t ticks = 0;

    /*Disable global interrupts*/
    __disable_irq();

    /*Load current ticks*/
    ticks = current_tick;

    /*Enable global interrupts*/
    __enable_irq();

    /*Return current ticks*/
    return ticks;
}

/**
 * @brief delay in milliseconds.
 * @param delay: Total ms to wait.
 * @retval None.
 */
void delay_ms(uint32_t delay)
{
    uint32_t start_time = get_tick();
    uint32_t wait_time = delay;

    /*Check if wait time is less than max delay*/
    if (wait_time < MAX_DELAY)
    {
    	/*Increase wait time*/
        wait_time += 1;
    }

    /*Wait until delay occurs*/
    while ((get_tick() - start_time) < wait_time) {}
}


/**
 * @brief Enables WD timer for safety protection.
 */
void iwdg_init(void)
{
	/*Enable IWDG*/
	IWDG->KR = 0x0000CCCC;

	/*Enable register access*/
	IWDG->KR = 0x00005555;

	/*Set the IWDG prescaler*/
	MODIFY_REG(IWDG->PR, IWDG_PR_PR, (0x06 << IWDG_PR_PR_Pos));

	/*Set the reload register to the largest value*/
	IWDG->RLR = 0xFFF;

	/*Wait for registers to be updated*/
	while ((IWDG->SR & (IWDG_SR_PVU | IWDG_SR_RVU | IWDG_SR_WVU)) != 0) {}

	/*Refresh watch dog*/
	IWDG->KR = 0x0000AAAA;
}


/**
 * @brief Refresh the WD counter
 */
void iwdg_refresh(void)
{
	/*Refresh watch dog*/
	IWDG->KR = 0x0000AAAA;
}
