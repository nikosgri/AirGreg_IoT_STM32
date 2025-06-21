/*
 * adc.c
 *
 *  Created on: Aug 17, 2024
 *      Author: Nikolaos Grigoriadis
 *      Email : n.grigoriadis09@gmail.com
 *      Title : Embedded software engineer
 *      Degree: BSc and MSc in computer science, university of Ioannina
 */

#include <adc.h>

/**
 * @function adc1_init
 *
 * @brief Enable ADC 1 channel to read internal temperature sensor values.
 * @note
 * - ADC1 is connected to APB2 bus.
 */
void adc1_init()
{
	/*Enable clock access to ADC1 peripheral*/
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

	/*Ensure that ADC1 is disabled first*/
	ADC1->CR &= ~ADC_CR_ADEN;

	/**** Configuration of ADC1 peripheral ****/

	/*Set the HSI as the ADC1 clock*/
	ADC1->CFGR2 &= ~ADC_CFGR2_CKMODE_0;
	ADC1->CFGR2 &= ~ADC_CFGR2_CKMODE_1;

	/*Set the ADC1 to one shot conversion mode*/
	ADC1->CFGR1 |= ADC_CFGR1_CONT;

	/*Select the scanning direction UP*/
	ADC1->CFGR1 |= ADC_CFGR1_SCANDIR;

	/*Select channel 18 for sensor reading*/
	ADC1->CHSELR |= ADC_CHSELR_CHSEL18;

	/*Select a sampling mode of 111 i.e. 239.5 ADC clk to be greater than 2.2us*/
	ADC1->SMPR |= ADC_SMPR_SMP;

	/*Set the TCEN bit in CCR register*/
	ADC->CCR |= ADC_CCR_TSEN;

	/**** End of configuration of ADC1 peripheral ****/

	/**** Calibration of ADC1 ****/

	/*Ensure that ADEN = 0 */
	if ((ADC1->CR & ADC_CR_ADEN) != 0)
	{
		/*Clear ADDEN bit*/
		ADC1->CR |= ADC_CR_ADDIS;
	}

	/*Enable software calibration*/
	ADC1->CR |= ADC_CR_ADCAL;

	/*Wait until EOCAL=1*/
	while ((ADC1->ISR & ADC_ISR_EOCAL) == 0) {}

	/*Clear EOCAL*/
	ADC1->ISR |= ADC_ISR_EOCAL;

	/****End of calibration of ADC1****/

	/**** Enable ADC1 peripheral ****/

	/*Clear the ready flag of ADC1*/
	ADC1->ISR |= ADC_ISR_ADRDY;

	/*Enable ADC1*/
	ADC1->CR |= ADC_CR_ADEN;

	/*Wait until ADC1 is fully enabled*/
	if ((ADC1->CFGR1 & ADC_CFGR1_AUTOFF) == 0)
	{
		while (!(ADC1->ISR & ADC_ISR_ADRDY)) {}
	}

	/**** End of enable ADC1 peripheral ****/
}

/**
 * @function adc1_read
 *
 * @brief Read the ADC DR register.
 * @retval TS_DATA, which is the actual temperature sensor output value converted by ADC.
 */
uint16_t adc1_read(void)
{
	/*Start conversion*/
	ADC1->CR |= ADC_CR_ADSTART;

    /*Wait for conversion to complete*/
    while (!(ADC1->ISR & ADC_ISR_EOC));

    /*Read the converted data*/
    return ADC1->DR;
}

/**
 * @function read_internal_temp
 *
 * @brief This function takes the temperature output value and converts it to celcius.
 * @retval Celcius data.
 */
int32_t read_internal_temp(int32_t ts_data)
{
	/*Local variable*/
	int32_t temperature;

	/*Reading the temperature, applied the formula provided in the datasheet*/
	temperature = ((ts_data * VDD_APPLI / VDD_CALIB) - (int32_t) * TEMP30_CAL_ADDR);

	temperature = temperature * (int32_t) (130 - 30);

	temperature = temperature / (int32_t) (*TEMP130_CAL_ADDR - * TEMP30_CAL_ADDR);

	temperature = temperature + 30;

	return temperature;
}

