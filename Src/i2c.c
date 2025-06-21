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

static void I2C1_gpio_init();

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


/**
 * @brief Transmits data over SDA line.
 * @param addr   : The slave address.
 * @param data   : Bytes to send.
 */
void I2C1_write_byte(uint8_t address, uint8_t data)
{
	/* Wait until the bus is not busy */
	while (I2C1->ISR & I2C_ISR_BUSY) {}

	/* Set the slave address */
	MODIFY_REG(I2C1->CR2, I2C_CR2_SADD, (address << 1U) << I2C_CR2_SADD_Pos);

	/* Set the addressing mode, to 7Bits*/
	I2C1->CR2 &= ~I2C_CR2_ADD10;

	/* 1 Byte is to be transmitted */
	MODIFY_REG(I2C1->CR2, I2C_CR2_NBYTES, (0x01 << I2C_CR2_NBYTES_Pos));

	/* Master requests a write transfer */
	I2C1->CR2 &= ~I2C_CR2_RD_WRN;

	/* Generate a start condition */
	I2C1->CR2 |= I2C_CR2_START;

	/* Wait until TXDR register is empty*/
	while (!(I2C1->ISR & I2C_ISR_TXIS)) {}

	/* Write data to TXDR register */
	I2C1->TXDR = (uint8_t) data;

	/* Wait until transfer is complete */
	while (!(I2C1->ISR & I2C_ISR_TC)) {}

	/* Generate a stop condition */
	I2C1->CR2 |= I2C_CR2_STOP;
}


int I2C1_write(uint8_t dev_addr, uint8_t reg_addr, const uint8_t *data, uint8_t size)
{
    uint32_t timeout;

    if (data == NULL || size == 0) return -1;

    timeout = 100000;
    while ((I2C1->ISR & I2C_ISR_BUSY) && timeout--);
    if (timeout == 0) return -1;

    // Step 1: Send register address + payload in one sequence
    I2C1->CR2 = 0;
    I2C1->CR2 |= (dev_addr << 1) << I2C_CR2_SADD_Pos;
    I2C1->CR2 |= (1 + size) << I2C_CR2_NBYTES_Pos; // 1 reg byte + data
    I2C1->CR2 &= ~I2C_CR2_RD_WRN; // Write
    I2C1->CR2 |= I2C_CR2_AUTOEND;
    I2C1->CR2 |= I2C_CR2_START;

    // First byte: register address
    while (!(I2C1->ISR & I2C_ISR_TXIS));
    I2C1->TXDR = reg_addr;

    // Remaining bytes: data
    for (uint8_t i = 0; i < size; i++) {
        while (!(I2C1->ISR & I2C_ISR_TXIS));
        I2C1->TXDR = data[i];
    }

    // Wait for stop flag
    while (!(I2C1->ISR & I2C_ISR_STOPF));
    I2C1->ICR |= I2C_ICR_STOPCF;

    return 0;
}



/**
 * @brief Receives 1 byte from a slave device over I2C1.
 * @param address : 7-bit I2C address of the slave device.
 * @param data    : Pointer to store the received byte.
 */
void I2C1_read_byte(uint8_t address, uint8_t *data)
{
	/* Wait if the bus is busy */
	while (I2C1->ISR & I2C_ISR_BUSY) {}

	/* Set the slave address */
	MODIFY_REG(I2C1->CR2, I2C_CR2_SADD, (address << 1) << I2C_CR2_SADD_Pos);

	/* Enable auto end mode */
	I2C1->CR2 |= I2C_CR2_AUTOEND;

	/* Set the number of bytes that needs to be written */
	MODIFY_REG(I2C1->CR2, I2C_CR2_NBYTES, (1U << I2C_CR2_NBYTES_Pos));

	/* Master requests a read direction */
	I2C1->CR2 |= I2C_CR2_RD_WRN;

	/* Generate a start condition */
	I2C1->CR2 |= I2C_CR2_START;

	/* Wait until there are data in the register */
	while (!(I2C1->ISR & I2C_ISR_RXNE)) {}

	/* Read the data */
	*data = I2C1->RXDR;

	/* Generate a stop condition */
	I2C1->CR2 |= I2C_CR2_STOP;
}


int I2C1_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t size)
{
    uint32_t timeout;

    if (buffer == NULL || size == 0) return -1;

    // Step 1: Wait until bus is free
    timeout = 100000;
    while ((I2C1->ISR & I2C_ISR_BUSY) && timeout--);
    if (timeout == 0) return -1;

    // Step 2: Send register address (write phase)
    I2C1->CR2 = 0;
    I2C1->CR2 |= (dev_addr << 1) << I2C_CR2_SADD_Pos;  // 7-bit address
    I2C1->CR2 |= 1 << I2C_CR2_NBYTES_Pos;              // Sending 1 byte
    I2C1->CR2 &= ~I2C_CR2_RD_WRN;                      // Write
    I2C1->CR2 |= I2C_CR2_START;

    while (!(I2C1->ISR & I2C_ISR_TXIS));
    I2C1->TXDR = reg_addr;
    while (!(I2C1->ISR & I2C_ISR_TC));

    // Step 3: Read N bytes
    I2C1->CR2 = 0;
    I2C1->CR2 |= (dev_addr << 1) << I2C_CR2_SADD_Pos;
    I2C1->CR2 |= size << I2C_CR2_NBYTES_Pos;
    I2C1->CR2 |= I2C_CR2_RD_WRN;    // Read
    I2C1->CR2 |= I2C_CR2_AUTOEND;
    I2C1->CR2 |= I2C_CR2_START;

    for (uint8_t i = 0; i < size; i++) {
        timeout = 100000;
        while (!(I2C1->ISR & I2C_ISR_RXNE) && timeout--);
        if (timeout == 0) return -1;
        buffer[i] = I2C1->RXDR;
    }

    return 0;
}


/**
 * @function I2C1_gpio_init
 *
 * @brief Initialize GPIO pins for I2C1 peripheral
 * @note
 *    - SCL->PB6->AF1
 *    - SDA->PB7->AF1
 */
static void I2C1_gpio_init()
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
 * @function I2C1_scan_bus
 *
 * @brief Scans the I2C bus for connected devices and prints the found addresses.
 * @note Assumes a simple ACK/NACK check on address probe.
 */
void I2C1_scan_bus(void)
{
    for (uint8_t addr = 0x08; addr <= 0x77; addr++)
    {
        // Wait until bus is free
        while (I2C1->ISR & I2C_ISR_BUSY) {}

        // Configure address and settings
        I2C1->CR2 = 0; // clear previous config
        I2C1->CR2 |= (addr << 1);            // 7-bit address
        I2C1->CR2 |= (0 << I2C_CR2_NBYTES_Pos); // send no data
        I2C1->CR2 &= ~I2C_CR2_RD_WRN;        // Write mode
        I2C1->CR2 &= ~I2C_CR2_AUTOEND;       // Manual STOP
        I2C1->CR2 |= I2C_CR2_START;

        // Wait for either NACK or TC (Transfer Complete)
        while (!(I2C1->ISR & (I2C_ISR_NACKF | I2C_ISR_TC))) {}

        if (!(I2C1->ISR & I2C_ISR_NACKF)) {
            // ACK received -> device found
            printf("Device found at 0x%02X\n", addr);
        }

        // Generate STOP and clear flags
        I2C1->CR2 |= I2C_CR2_STOP;
        while (!(I2C1->ISR & I2C_ISR_STOPF)) {}

        I2C1->ICR |= I2C_ICR_STOPCF | I2C_ICR_NACKCF;
    }
}
