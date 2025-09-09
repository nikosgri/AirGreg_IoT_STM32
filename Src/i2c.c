/*
 * i2c.c
 *
 *  Created on: Sep 22, 2024
 *      Author: Nikolaos Grigoriadis
 *      Email : n.grigoriadis09@gmail.com
 *      Title : Embedded software engineer
 *      Degree: BSc and MSc in computer science, university of Ioannina
 */


#include <i2c.h>



//////////////////////////////////
// I2C Peripherals initialization
/////////////////////////////////

/**
 * @function I2C1_init
 *
 * @brief Initialize I2C1 peripheral.
 * @note TIMINGR register calculated with CUBEMX, based on
 *     - Fast mode 400 kHz
 *     - Rise time 100ns
 *     - Fall time 10ns
 *     - Periph clock 16MHz clock.
 */
void I2C1_init(void)
{
    /*Initialize GPIO pins for I2C1 peripheral*/
    I2C1_gpio_init();

    /*Enable clock access to I2C1 peripheral*/
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    /*Reset I2C1 peripheral*/
    RCC->APB1RSTR |= RCC_APB1RSTR_I2C1RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C1RST;

    /*Disable automatic END mode*/
    I2C1->CR2 &= ~I2C_CR2_AUTOEND;

    /*Program the TIMINGS Register*/
    I2C1->TIMINGR = 0x00303D5B;

    /*Enable the peripheral*/
    I2C1->CR1 |= I2C_CR1_PE;
}



//////////////////////
// API I2C Functions
/////////////////////


/**
 * @brief Transmits data over SDA line.
 * @param i2c    : Pointer to I2C structure.
 * @param addr   : The slave address.
 * @param data   : Bytes to send.
 */
void I2Cx_write_byte(I2C_TypeDef *i2c, uint8_t address, uint8_t data)
{
	/* Wait until the bus is not busy */
	while (i2c->ISR & I2C_ISR_BUSY) {}

	/* Set the slave address */
	MODIFY_REG(i2c->CR2, I2C_CR2_SADD, (address << 1U) << I2C_CR2_SADD_Pos);

	/* Set the addressing mode, to 7Bits*/
	i2c->CR2 &= ~I2C_CR2_ADD10;

	/* 1 Byte is to be transmitted */
	MODIFY_REG(i2c->CR2, I2C_CR2_NBYTES, (0x01 << I2C_CR2_NBYTES_Pos));

	/* Master requests a write transfer */
	i2c->CR2 &= ~I2C_CR2_RD_WRN;

	/* Generate a start condition */
	i2c->CR2 |= I2C_CR2_START;

	/* Wait until TXDR register is empty*/
	while (!(i2c->ISR & I2C_ISR_TXIS)) {}

	/* Write data to TXDR register */
	i2c->TXDR = (uint8_t) data;

	/* Wait until transfer is complete */
	while (!(i2c->ISR & I2C_ISR_TC)) {}

	/* Generate a stop condition */
	i2c->CR2 |= I2C_CR2_STOP;
}


/**
 * @brief Write multiple bytes to a device register.
 *
 * @param i2c I2C peripheral pointer.
 * @param dev_addr Device 7-bit address.
 * @param reg_addr Register address to write to.
 * @param data Pointer to data buffer.
 * @param size Number of bytes to write.
 * @retval 0 on success, -1 on failure.
 */
int I2Cx_write(I2C_TypeDef *i2c, uint8_t dev_addr, uint8_t reg_addr, const uint8_t *data, uint8_t size)
{
    uint32_t timeout;

    if (data == NULL || size == 0) return -1;

    timeout = 100000;
    while ((i2c->ISR & I2C_ISR_BUSY) && timeout--);
    if (timeout == 0) return -1;

    // Step 1: Send register address + payload in one sequence
    i2c->CR2 = 0;
    i2c->CR2 |= (dev_addr << 1) << I2C_CR2_SADD_Pos;
    i2c->CR2 |= (1 + size) << I2C_CR2_NBYTES_Pos; // 1 reg byte + data
    i2c->CR2 &= ~I2C_CR2_RD_WRN; // Write
    i2c->CR2 |= I2C_CR2_AUTOEND;
    i2c->CR2 |= I2C_CR2_START;

    // First byte: register address
    while (!(i2c->ISR & I2C_ISR_TXIS));
    i2c->TXDR = reg_addr;

    // Remaining bytes: data
    for (uint8_t i = 0; i < size; i++) {
        while (!(i2c->ISR & I2C_ISR_TXIS));
        i2c->TXDR = data[i];
    }

    // Wait for stop flag
    while (!(i2c->ISR & I2C_ISR_STOPF));
    i2c->ICR |= I2C_ICR_STOPCF;

    return 0;
}


/**
 * @brief Receives 1 byte from a slave device over I2C1.
 * @param i2c     : Pointer to I2C structure.
 * @param address : 7-bit I2C address of the slave device.
 * @param data    : Pointer to store the received byte.
 */
void I2Cx_read_byte(I2C_TypeDef *i2c, uint8_t address, uint8_t *data)
{
	/* Wait if the bus is busy */
	while (i2c->ISR & I2C_ISR_BUSY) {}

	/* Set the slave address */
	MODIFY_REG(i2c->CR2, I2C_CR2_SADD, (address << 1) << I2C_CR2_SADD_Pos);

	/* Enable auto end mode */
	i2c->CR2 |= I2C_CR2_AUTOEND;

	/* Set the number of bytes that needs to be written */
	MODIFY_REG(i2c->CR2, I2C_CR2_NBYTES, (1U << I2C_CR2_NBYTES_Pos));

	/* Master requests a read direction */
	i2c->CR2 |= I2C_CR2_RD_WRN;

	/* Generate a start condition */
	i2c->CR2 |= I2C_CR2_START;

	/* Wait until there are data in the register */
	while (!(i2c->ISR & I2C_ISR_RXNE)) {}

	/* Read the data */
	*data = i2c->RXDR;

	/* Generate a stop condition */
	i2c->CR2 |= I2C_CR2_STOP;
}


/**
 * @brief Read multiple bytes from a device register.
 *
 * @param i2c I2C peripheral pointer.
 * @param dev_addr Device 7-bit address.
 * @param reg_addr Register address to read from.
 * @param buffer Buffer to store read data.
 * @param size Number of bytes to read.
 * @retval 0 on success, -1 on failure.
 */
int I2Cx_read(I2C_TypeDef *i2c, uint8_t dev_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t size)
{
    uint32_t timeout;

    if (buffer == NULL || size == 0) return -1;

    // Step 1: Wait until bus is free
    timeout = 100000;
    while ((i2c->ISR & I2C_ISR_BUSY) && timeout--);
    if (timeout == 0) return -1;

    // Step 2: Send register address (write phase)
    i2c->CR2 = 0;
    i2c->CR2 |= (dev_addr << 1) << I2C_CR2_SADD_Pos;  // 7-bit address
    i2c->CR2 |= 1 << I2C_CR2_NBYTES_Pos;              // Sending 1 byte
    i2c->CR2 &= ~I2C_CR2_RD_WRN;                      // Write
    i2c->CR2 |= I2C_CR2_START;

    while (!(i2c->ISR & I2C_ISR_TXIS));
    i2c->TXDR = reg_addr;
    while (!(i2c->ISR & I2C_ISR_TC));

    // Step 3: Read N bytes
    i2c->CR2 = 0;
    i2c->CR2 |= (dev_addr << 1) << I2C_CR2_SADD_Pos;
    i2c->CR2 |= size << I2C_CR2_NBYTES_Pos;
    i2c->CR2 |= I2C_CR2_RD_WRN;    // Read
    i2c->CR2 |= I2C_CR2_AUTOEND;
    i2c->CR2 |= I2C_CR2_START;

    for (uint8_t i = 0; i < size; i++) {
        timeout = 100000;
        while (!(i2c->ISR & I2C_ISR_RXNE) && timeout--);
        if (timeout == 0) return -1;
        buffer[i] = i2c->RXDR;
    }

    return 0;
}


/**
 * @brief Scan the I2C bus and print detected device addresses.
 *
 * @param i2c I2C peripheral pointer.
 */
void I2Cx_scan_bus(I2C_TypeDef *i2c)
{
    for (uint8_t addr = 0x08; addr <= 0x77; addr++)
    {
        // Wait until bus is free
        while (i2c->ISR & I2C_ISR_BUSY) {}

        // Configure address and settings
        i2c->CR2 = 0; // clear previous config
        i2c->CR2 |= (addr << 1);            // 7-bit address
        i2c->CR2 |= (0 << I2C_CR2_NBYTES_Pos); // send no data
        i2c->CR2 &= ~I2C_CR2_RD_WRN;        // Write mode
        i2c->CR2 &= ~I2C_CR2_AUTOEND;       // Manual STOP
        i2c->CR2 |= I2C_CR2_START;

        // Wait for either NACK or TC (Transfer Complete)
        while (!(i2c->ISR & (I2C_ISR_NACKF | I2C_ISR_TC))) {}

        if (!(i2c->ISR & I2C_ISR_NACKF)) {
            // ACK received -> device found
            printf("Device found at 0x%02X\n", addr);
        }

        // Generate STOP and clear flags
        i2c->CR2 |= I2C_CR2_STOP;
        while (!(i2c->ISR & I2C_ISR_STOPF)) {}

        i2c->ICR |= I2C_ICR_STOPCF | I2C_ICR_NACKCF;
    }
}
