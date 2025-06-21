/*
 * adc.h
 *
 *  Created on: Aug 17, 2024
 *      Author: Nikolaos Grigoriadis
 *      Email : n.grigoriadis09@gmail.com
 *      Title : Embedded software engineer
 *      Degree: BSc and MSc in computer science, university of Ioannina
 */

#ifndef ADC_H_
#define ADC_H_

#include <main.h>

/*Calibration value. Raw data acquired at temperature of 130 °C, VDDA = 3V*/
#define TEMP130_CAL_ADDR            ((uint16_t*) ((uint32_t) 0x1FF8007E))
/*Calibration value. Raw data acquired at temperature of 30 °C, VDDA = 3V*/
#define TEMP30_CAL_ADDR             ((uint16_t*) ((uint32_t) 0x1FF8007A))
/*Factory-calibrated reference voltage in millivolts (mV) used during temperature sensor calibration*/
#define VDD_CALIB                   ((uint16_t) (300))
/*Actual applied supply voltage in millivolts (mV) used in the system*/
#define VDD_APPLI                   ((uint16_t) (330))



void adc1_init();
uint16_t adc1_read(void);
int32_t read_internal_temp(int32_t ts_data);

#endif /* ADC_H_ */
