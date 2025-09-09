/*
 * device.c
 *
 *  Created on: Feb 20, 2025
 *      Author: Nikolaos Grigoriadis
 *      Email : n.grigoriadis09@gmail.com
 *      Title : Embedded software engineer
 *      Degree: BSc and MSc in computer science, university of Ioannina
 */


#include <device.h>



/*Global variables*/
volatile CoreFlags_t device;
volatile int sequence_number;
volatile int keep_alive = 60;
volatile long int pulse1_counter = 0;
CircularBuffer cb;
air_greg_dev_t air_greg;

void pulse1_init(void)
{
    /* Enable clock access to GPIOC */
    RCC->IOPENR |= RCC_IOPENR_GPIOCEN;

    /* Enable SYSCFG clock */
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    /* Set PC13 as input */
    GPIOC->MODER &= ~(GPIO_MODER_MODE13_0 | GPIO_MODER_MODE13_1);

    /* Select output push-pull type (not necessary for input, but keeping for consistency) */
    GPIOC->OTYPER &= ~GPIO_OTYPER_OT_13;

    /* Set GPIO speed to high */
    GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEED13_1;
    GPIOC->OSPEEDR &= ~GPIO_OSPEEDER_OSPEED13_0;

    /* Enable internal pull-up */
    GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD13;  // Clear both bits
    GPIOC->PUPDR |= GPIO_PUPDR_PUPD13_0; // Set pull-up

    /* Configure EXTI13 on PC13 */
    MODIFY_REG(SYSCFG->EXTICR[3], SYSCFG_EXTICR4_EXTI13, (0x02 << SYSCFG_EXTICR4_EXTI13_Pos));

    /* Enable EXTI13 interrupt */
    EXTI->IMR |= EXTI_IMR_IM13;

    /* Enable falling-edge trigger detection */
    EXTI->RTSR &= ~EXTI_RTSR_RT13;  // Ensure rising edge is disabled
    EXTI->FTSR |= EXTI_FTSR_FT13;   // Use falling edge to wake up

    /* Clear EXTI13 pending flag */
    EXTI->PR = EXTI_PR_PIF13;

    /* Enable EXTI4_15 Interrupt in NVIC */
    NVIC_EnableIRQ(EXTI4_15_IRQn);
    NVIC_SetPriority(EXTI4_15_IRQn, 0); //High priority
}


/**
 * @brief Initializes the circular buffer.
 *
 * This function resets the head, tail, and size of the buffer to zero.
 *
 * @param cb Pointer to the circular buffer.
 */
void CircularBuffer_Init(CircularBuffer *cb) {
    cb->head = 0;
    cb->tail = 0;
    cb->size = 0;
}


/**
 * @brief Checks if the circular buffer is empty.
 *
 * @param cb Pointer to the circular buffer.
 * @return true if the buffer is empty, false otherwise.
 */
bool CircularBuffer_IsEmpty(CircularBuffer *cb) {
    return cb->size == 0;
}


/**
 * @brief Checks if the circular buffer is full.
 *
 * @param cb Pointer to the circular buffer.
 * @return true if the buffer is full, false otherwise.
 */
bool CircularBuffer_IsFull(CircularBuffer *cb) {
    return cb->size == NUM_PAYLOADS;
}


/**
 * @brief Pushes a payload to the circular buffer.
 *
 * This function adds a payload to the head of the buffer.
 * If the buffer is full, the operation is rejected.
 *
 * @param cb Pointer to the circular buffer.
 * @param payload Pointer to the payload string to push.
 * @return true if the payload was successfully pushed, false if the buffer is full.
 */
bool CircularBuffer_Push(CircularBuffer *cb, const char *payload) {
    if (CircularBuffer_IsFull(cb)) {
        return false;  // Buffer is full, cannot push
    }

    // Copy the payload to the buffer
    strncpy(cb->buffer[cb->head], payload, PAYLOAD_SIZE - 1);
    cb->buffer[cb->head][PAYLOAD_SIZE - 1] = '\0';  // Ensure null-termination
    cb->head = (cb->head + 1) % NUM_PAYLOADS;  // Wrap around if necessary
    cb->size++;
    return true;
}


/**
 * @brief Pops a payload from the circular buffer.
 *
 * This function removes the oldest payload from the tail of the buffer.
 * If the buffer is empty, the operation is rejected.
 *
 * @param cb Pointer to the circular buffer.
 * @param payload Pointer to the buffer where the popped payload will be stored.
 * @return true if the payload was successfully popped, false if the buffer is empty.
 */
bool CircularBuffer_Pop(CircularBuffer *cb, char *payload) {
    if (CircularBuffer_IsEmpty(cb)) {
        return false;  // Buffer is empty, nothing to pop
    }

    // Copy the payload to the provided pointer
    strncpy(payload, cb->buffer[cb->tail], PAYLOAD_SIZE - 1);
    payload[PAYLOAD_SIZE - 1] = '\0';  // Ensure null-termination
    cb->tail = (cb->tail + 1) % NUM_PAYLOADS;  // Wrap around if necessary
    cb->size--;
    return true;
}
