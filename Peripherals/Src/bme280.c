/*
 * bme280.c
 *
 *  Created on: Jun 16, 2025
 *      Author: Nikolaos Grigoriadis
 *      Email : n.grigoriadis09@gmail.com
 *      Title : Embedded software engineer
 *      Degree: BSc and MSc in computer science, university of Ioannina
 *      Description: Temperature & Humidity measurements
 */


#include "bme280.h"



static int bme280_is_measuring(bme280_t *dev);
static int bme280_wait_for_measurement(bme280_t *dev);
static int bme280_read_raw_data(bme280_t *dev, int32_t *raw_temp, int32_t *raw_hum);
static float bme280_compensate_temperature(bme280_t *dev, int32_t raw_temp);
static float bme280_compensate_humidity(bme280_t *dev, int32_t raw_hum);


/**
 * @brief Initialize the BME280 sensor and apply default configuration.
 *
 * @param dev Pointer to BME280 device structure.
 * @return BME280_OK on success, error code otherwise.
 */
int bme280_init(bme280_t *dev)
{
    int ret;

    // Read chip ID to verify communication
    ret = bme280_read_chip_id();
    if (ret != BME280_OK) {
        return ret;
    }

    // Reset the device
    ret = bme280_soft_reset();
    if (ret != BME280_OK) {
        return ret;
    }

    // Read calibration data
    ret = bme280_read_calibration_data(dev);
    if (ret != BME280_OK) {
        return BME280_ERR;
    }

    // Set default settings if not already set
    if (dev->temp_oversampling == 0) {
        dev->temp_oversampling = BME280_OVERSAMPLING_1X;
    }
    if (dev->hum_oversampling == 0) {
        dev->hum_oversampling = BME280_OVERSAMPLING_1X;
    }
    if (dev->filter == 0) {
        dev->filter = BME280_FILTER_OFF;
    }
    dev->mode = BME280_FORCED_MODE;

    // Apply settings
    return bme280_set_settings(dev);
}


/**
 * @brief Read and validate the BME280 chip ID.
 *
 * @return BME280_OK if chip ID is valid, BME280_ERR otherwise.
 */
bme280_status bme280_read_chip_id(void)
{
	/*Local variables*/
	uint8_t chip_id = 0;

	/* Get chip ID */
//	I2C1_write_byte(BME280_I2C_ADDR, BME280_REG_ID);
	I2Cx_write_byte(I2C1, BME280_I2C_ADDR, BME280_REG_ID);

	/* Read chip ID */
//	I2C1_read_byte(BME280_I2C_ADDR, &chip_id);
	I2Cx_read_byte(I2C1, BME280_I2C_ADDR, &chip_id);

	/* Check if the chip ID is correct */
	if (chip_id != BME280_CHIP_ID)
	{
		LOG_ERR("Invalid chip ID");
		return BME280_ERR;
	}

	LOG_INF("Correct chip ID");

	return BME280_OK;
}


/**
 * @brief Perform a soft reset of the BME280 sensor.
 *
 * @return BME280_OK on success, error code otherwise.
 */
bme280_status bme280_soft_reset(void)
{
	uint8_t cmd = BME280_RESET_CMD;
//	int ret = I2C1_write(BME280_I2C_ADDR, BME280_REG_RESET, &cmd, 1);
	int ret = I2Cx_write(I2C1, BME280_I2C_ADDR, BME280_REG_RESET, &cmd, 1);
	if (ret != BME280_OK) {
		return ret;
	}

	// Wait for reset to complete
	delay_ms(10);
	return BME280_OK;
}


/**
 * @brief Read temperature and humidity calibration data from the sensor.
 *
 * @param dev Pointer to BME280 device structure.
 * @return BME280_OK on success, error code otherwise.
 */
bme280_status bme280_read_calibration_data(bme280_t *dev)
{
    uint8_t calib_data[33];  // 26 bytes from 0x88, 7 bytes from 0xE1
    int ret;

    // Read 0x88–0xA1 (26 bytes)
//    ret = I2C1_read(BME280_I2C_ADDR, 0x88, calib_data, 26);
    ret = I2Cx_read(I2C1, BME280_I2C_ADDR, 0x88, calib_data, 26);
    if (ret != BME280_OK) {
        return ret;
    }

    // Read 0xE1–0xE7 (7 bytes)
//    ret = I2C1_read(BME280_I2C_ADDR, 0xE1, calib_data + 26, 7);
    ret = I2Cx_read(I2C1, BME280_I2C_ADDR, 0xE1, calib_data + 26, 7);
    if (ret != BME280_OK) {
        return ret;
    }

    // Parse temperature calibration data
    dev->calib.dig_T1 = (uint16_t)((calib_data[1] << 8) | calib_data[0]);
    dev->calib.dig_T2 = (int16_t)((calib_data[3] << 8) | calib_data[2]);
    dev->calib.dig_T3 = (int16_t)((calib_data[5] << 8) | calib_data[4]);

    // Parse humidity calibration data
    dev->calib.dig_H1 = calib_data[25];
    dev->calib.dig_H2 = (int16_t)((calib_data[27] << 8) | calib_data[26]);
    dev->calib.dig_H3 = calib_data[28];
    dev->calib.dig_H4 = (int16_t)((calib_data[29] << 4) | (calib_data[30] & 0x0F));
    dev->calib.dig_H5 = (int16_t)((calib_data[31] << 4) | ((calib_data[30] >> 4) & 0x0F));
    dev->calib.dig_H6 = (int8_t)calib_data[32];

    return BME280_OK;
}


/**
 * @brief Configure BME280 sensor settings (oversampling, filter, mode).
 *
 * @param dev Pointer to BME280 device structure.
 * @return BME280_OK on success, error code otherwise.
 */
int bme280_set_settings(bme280_t *dev)
{
    uint8_t ctrl_hum, ctrl_meas, config;
    int ret;

    // Set humidity control register
    ctrl_hum = dev->hum_oversampling & 0x07;
//    ret = I2C1_write(BME280_I2C_ADDR, BME280_REG_CTRL_HUM, &ctrl_hum, 1);
    ret = I2Cx_write(I2C1, BME280_I2C_ADDR, BME280_REG_CTRL_HUM, &ctrl_hum, 1);
    if (ret != BME280_OK) {
        return ret;
    }

    // Set config register (filter and standby time)
    config = ((0x00 << 5) | (dev->filter << 2) | 0x00); // Standby time = 0.5ms
//    ret = I2C1_write(BME280_I2C_ADDR, BME280_REG_CONFIG, &config, 1);
    ret = I2Cx_write(I2C1, BME280_I2C_ADDR, BME280_REG_CONFIG, &config, 1);
    if (ret != BME280_OK) {
        return ret;
    }

    // Set measurement control register
    ctrl_meas = ((dev->temp_oversampling << 5) | (0x00 << 2) | dev->mode);
//    return I2C1_write(BME280_I2C_ADDR, BME280_REG_CTRL_MEAS, &ctrl_meas, 1);
    return I2Cx_write(I2C1, BME280_I2C_ADDR, BME280_REG_CTRL_MEAS, &ctrl_meas, 1);
}


/**
 * @brief Trigger a single forced-mode measurement.
 *
 * @param dev Pointer to BME280 device structure.
 * @return BME280_OK on success, error code otherwise.
 */
int bme280_trigger_measurement(bme280_t *dev)
{
    uint8_t ctrl_meas;

    if (dev->mode != BME280_FORCED_MODE) {
        return BME280_ERR;
    }

    // Read current ctrl_meas register
//    int ret = I2C1_read(BME280_I2C_ADDR, BME280_REG_CTRL_MEAS, &ctrl_meas, 1);
    int ret = I2Cx_read(I2C1, BME280_I2C_ADDR, BME280_REG_CTRL_MEAS, &ctrl_meas, 1);
    if (ret != BME280_OK) {
        return ret;
    }

    // Set mode to forced mode
    ctrl_meas &= ~0x03; // Clear mode bits
    ctrl_meas |= BME280_FORCED_MODE;

    // Write back to trigger measurement
//    return I2C1_write(BME280_I2C_ADDR, BME280_REG_CTRL_MEAS, &ctrl_meas, 1);
    return I2Cx_write(I2C1, BME280_I2C_ADDR, BME280_REG_CTRL_MEAS, &ctrl_meas, 1);
}


/**
 * @brief Read a single set of temperature and humidity measurements.
 *
 * @param dev Pointer to BME280 device structure.
 * @param temperature Pointer to store compensated temperature.
 * @param humidity Pointer to store compensated humidity.
 * @return BME280_OK on success, error code otherwise.
 */
int bme280_read_temperature_and_humidity(bme280_t *dev, float *temperature, float *humidity) {
    int32_t raw_temp, raw_hum;
    int ret;

    // Trigger measurement
    ret = bme280_trigger_measurement(dev);
    if (ret != BME280_OK) {
        return ret;
    }

    // Wait for measurement to complete
    ret = bme280_wait_for_measurement(dev);
    if (ret != BME280_OK) {
        return ret;
    }

    // Read raw data
    ret = bme280_read_raw_data(dev, &raw_temp, &raw_hum);
    if (ret != BME280_OK) {
        return ret;
    }

    // Compensate data
    *temperature = bme280_compensate_temperature(dev, raw_temp);
    *humidity = bme280_compensate_humidity(dev, raw_hum);

    return BME280_OK;
}


/**
 * @brief Read multiple averaged temperature and humidity measurements.
 *
 * @param dev Pointer to BME280 device structure.
 * @param temperature Pointer to store averaged temperature.
 * @param humidity Pointer to store averaged humidity.
 * @param samples Number of samples to average.
 * @return BME280_OK on success, error code otherwise.
 */
int bme280_read_averaged_measurements(bme280_t *dev, float *temperature, float *humidity, uint8_t samples) {
    float temp_sum = 0, hum_sum = 0;
    float temp, hum;
    int ret;

    if (samples == 0) {
        samples = 1;
    }

    for (uint8_t i = 0; i < samples; i++) {
        ret = bme280_read_temperature_and_humidity(dev, &temp, &hum);
        if (ret != BME280_OK) {
            return ret;
        }

        temp_sum += temp;
        hum_sum += hum;
    }

    *temperature = temp_sum / samples;
    *humidity = hum_sum / samples;

    return BME280_OK;
}


/**
 * @brief Check if a measurement is currently in progress.
 *
 * @param dev Pointer to BME280 device structure.
 * @return 1 if measuring, 0 if idle, negative on error.
 */
static int bme280_is_measuring(bme280_t *dev) {
    uint8_t status;
//    int ret = I2C1_read(BME280_I2C_ADDR, BME280_REG_STATUS, &status, 1);
    int ret = I2Cx_read(I2C1, BME280_I2C_ADDR, BME280_REG_STATUS, &status, 1);
    if (ret != BME280_OK) {
        return ret;
    }
    return (status & BME280_STATUS_MEASURING) ? 1 : 0;
}


/**
 * @brief Wait until the measurement is complete or timeout occurs.
 *
 * @param dev Pointer to BME280 device structure.
 * @return BME280_OK on success, BME280_ERR on timeout or failure.
 */
static int bme280_wait_for_measurement(bme280_t *dev) {
    uint32_t timeout = BME280_MEASUREMENT_TIMEOUT;
    int measuring;

    do {
        delay_ms(2);
        measuring = bme280_is_measuring(dev);
        if (measuring < 0) {
            return measuring;
        }
        timeout -= 2;
    } while (measuring && timeout > 0);

    return (timeout > 0) ? BME280_OK : BME280_ERR;
}


/**
 * @brief Read raw temperature and humidity values from sensor.
 *
 * @param dev Pointer to BME280 device structure.
 * @param raw_temp Pointer to store raw temperature.
 * @param raw_hum Pointer to store raw humidity.
 * @return BME280_OK on success, error code otherwise.
 */
static int bme280_read_raw_data(bme280_t *dev, int32_t *raw_temp, int32_t *raw_hum) {
    uint8_t data[8];
    int ret;

    // Read all data registers (pressure MSB to humidity LSB)
//    ret = I2C1_read(BME280_I2C_ADDR, BME280_REG_PRESS_MSB, data, 8);
    ret = I2Cx_read(I2C1, BME280_I2C_ADDR, BME280_REG_PRESS_MSB, data, 8);
    if (ret != BME280_OK) {
        return ret;
    }

    // Combine temperature data (20 bits)
    *raw_temp = (int32_t)(((uint32_t)data[3] << 12) | ((uint32_t)data[4] << 4) | ((uint32_t)data[5] >> 4));

    // Combine humidity data (16 bits)
    *raw_hum = (int32_t)(((uint32_t)data[6] << 8) | (uint32_t)data[7]);

    return BME280_OK;
}


/**
 * @brief Apply temperature compensation algorithm to raw value.
 *
 * @param dev Pointer to BME280 device structure.
 * @param raw_temp Raw temperature reading.
 * @return Compensated temperature in degrees Celsius.
 */
static float bme280_compensate_temperature(bme280_t *dev, int32_t raw_temp) {
    int32_t var1, var2;
    float temperature;

    var1 = ((((raw_temp >> 3) - ((int32_t)dev->calib.dig_T1 << 1))) * ((int32_t)dev->calib.dig_T2) >> 11);
    var2 = (((((raw_temp >> 4) - ((int32_t)dev->calib.dig_T1)) * ((raw_temp >> 4) - ((int32_t)dev->calib.dig_T1))) >> 12) * ((int32_t)dev->calib.dig_T3) >> 14);

    dev->calib.t_fine = var1 + var2;
    temperature = (float)((dev->calib.t_fine * 5 + 128) >> 8) / 100.0f;

    return temperature;
}


/**
 * @brief Apply humidity compensation algorithm to raw value.
 *
 * @param dev Pointer to BME280 device structure.
 * @param raw_hum Raw humidity reading.
 * @return Compensated relative humidity in %RH.
 */
static float bme280_compensate_humidity(bme280_t *dev, int32_t raw_hum) {
    int32_t v_x1;

    v_x1 = (dev->calib.t_fine - ((int32_t)76800));
    v_x1 = (((((raw_hum << 14) - ((int32_t)dev->calib.dig_H4 << 20) -
              ((int32_t)dev->calib.dig_H5 * v_x1)) + ((int32_t)16384)) >> 15) *
              (((((((v_x1 * ((int32_t)dev->calib.dig_H6)) >> 10) *
              (((v_x1 * ((int32_t)dev->calib.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) +
              ((int32_t)2097152)) * ((int32_t)dev->calib.dig_H2) + 8192) >> 14));

    v_x1 = (v_x1 - (((((v_x1 >> 15) * (v_x1 >> 15)) >> 7) * ((int32_t)dev->calib.dig_H1)) >> 4));
    v_x1 = (v_x1 < 0) ? 0 : v_x1;
    v_x1 = (v_x1 > 419430400) ? 419430400 : v_x1;

    return (float)(v_x1 >> 12) / 1024.0f;
}

