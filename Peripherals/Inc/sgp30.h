/*
 * sgp30.h
 *
 *  Created on: Jun 17, 2025
 *      Author: Nikolaos Grigoriadis
 *      Email : n.grigoriadis09@gmail.com
 *      Title : Embedded software engineer
 *      Degree: BSc and MSc in computer science, university of Ioannina
 */

#ifndef INC_SGP30_H_
#define INC_SGP30_H_

/* I2C Chip Address */
#define SGP30_I2C_ADDR   0x58

/* Commands */
#define INIT_AIR_QUALITY_CMD     0x2003
#define MEASURE_AIR_QUALITY_CMD  0x2008
#define GET_BASELINE_CMD         0x2015
#define SET_BASELINE_CMD         0x201E
#define SET_HUMIDITY_CMD         0x2061
#define MEASURE_TEST_CMD         0x2032
#define GET_FEATURE_SET_VER_CMD  0x202F
#define MEASURE_RAW_SIGNALS_CMD  0x2050
#define SOFTWARE_RESET_CMD       0x0006
#define CHIP_ID_CMD              0x3682


#endif /* INC_SGP30_H_ */
