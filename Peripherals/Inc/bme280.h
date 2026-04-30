/*
 * bme280.h
 *
 *  Created on: Jun 16, 2025
 *      Author: grego
 */

#ifndef INC_BME280_H_
#define INC_BME280_H_


#include "main.h"
#include "logger.h"
#include "i2c.h"


/* Chip address */
#define BME280_I2C_ADDR 0x77

/* Register addresses */
#define BME280_REG_ID         0xD0
#define BME280_REG_RESET      0xE0
#define BME280_REG_CTRL_HUM   0xF2
#define BME280_REG_STATUS     0xF3
#define BME280_REG_CTRL_MEAS  0xF4
#define BME280_REG_CONFIG     0xF5
#define BME280_REG_PRESS_MSB  0xF7
#define BME280_REG_PRESS_LSB  0xF8
#define BME280_REG_PRESS_XLSB 0xF9
#define BME280_REG_TEMP_MSB   0xFA
#define BME280_REG_TEMP_LSB   0xFB
#define BME280_REG_TEMP_XLSB  0xFC
#define BME280_REG_HUM_MSB    0xFD
#define BME280_REG_HUM_LSB    0xFE
#define BME280_REG_CALIB00    0x88
#define BME280_REG_CALIB25    0xA1
#define BME280_REG_CALIB26    0xE1

/* Constants */
#define BME280_CHIP_ID        0x60
#define BME280_RESET_CMD      0xB6

/* Status bit masks */
#define BME280_STATUS_MEASURING (1 << 3)
#define BME280_STATUS_IM_UPDATE (1 << 0)

/* Timeout for measurement completion (ms) */
#define BME280_MEASUREMENT_TIMEOUT 100


/**
 * @enum bme280_status
 * @brief Status return codes for BME280 functions.
 */
typedef enum {
	BME280_OK, /**< Operation successful */
	BME280_ERR /**< Operation failed */
}bme280_status;


/**
 * @enum bme280_mode_t
 * @brief Available operating modes of the BME280 sensor.
 */
typedef enum {
    BME280_SLEEP_MODE = 0, /**< Sleep mode */
    BME280_FORCED_MODE = 1, /**< Forced mode (one measurement per trigger) */
    BME280_NORMAL_MODE = 3  /**< Continuous measurement */
} bme280_mode_t;


/**
 * @enum bme280_oversampling_t
 * @brief Oversampling settings for temperature and humidity measurements.
 */
typedef enum {
    BME280_SKIPPED = 0,
    BME280_OVERSAMPLING_1X = 1,
    BME280_OVERSAMPLING_2X = 2,
    BME280_OVERSAMPLING_4X = 3,
    BME280_OVERSAMPLING_8X = 4,
    BME280_OVERSAMPLING_16X = 5
} bme280_oversampling_t;


/**
 * @enum bme280_filter_t
 * @brief IIR filter coefficients for BME280 data filtering.
 */
typedef enum {
    BME280_FILTER_OFF = 0,
    BME280_FILTER_2 = 1,
    BME280_FILTER_4 = 2,
    BME280_FILTER_8 = 3,
    BME280_FILTER_16 = 4
} bme280_filter_t;


/**
 * @struct bme280_calib_data_t
 * @brief Calibration data structure for the BME280 sensor.
 */
typedef struct {
    uint16_t dig_T1;
    int16_t dig_T2, dig_T3;
    uint16_t dig_H1;
    int16_t dig_H2;
    uint8_t dig_H3;
    int16_t dig_H4, dig_H5;
    int8_t dig_H6;
    int32_t t_fine;
} bme280_calib_data_t;


/**
 * @struct bme280_t
 * @brief Main structure for BME280 device configuration and calibration.
 */
typedef struct {
    // Calibration data
    bme280_calib_data_t calib; /**< Calibration data */

    // Settings
    bme280_mode_t mode; /**< Sensor operating mode */
    bme280_oversampling_t temp_oversampling; /**< Temperature oversampling setting */
    bme280_oversampling_t hum_oversampling;  /**< Humidity oversampling setting */
    bme280_filter_t filter; /**< IIR filter setting */
} bme280_t;

bme280_status bme280_read_chip_id(void);
bme280_status bme280_read_calibration_data(bme280_t *dev);
bme280_status bme280_soft_reset(void);
int bme280_init(bme280_t *dev);
int bme280_set_settings(bme280_t *dev);
int bme280_trigger_measurement(bme280_t *dev);
int bme280_read_temperature_and_humidity(bme280_t *dev, float *temperature, float *humidity);
int bme280_read_averaged_measurements(bme280_t *dev, float *temperature, float *humidity, uint8_t samples);


#endif /* INC_BME280_H_ */
