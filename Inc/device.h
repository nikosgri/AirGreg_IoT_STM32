/*
 * device.h
 *
 *  Created on: Feb 20, 2025
 *      Author: grego
 */

#ifndef DEVICE_H_
#define DEVICE_H_

#include "rtc.h"
#include "swo.h"
#include "main.h"
#include "stdbool.h"

#define PAYLOAD_SIZE 156    // Max size of a single payload string
#define NUM_PAYLOADS 5     // Number of payloads to store

typedef struct {
    char buffer[NUM_PAYLOADS][PAYLOAD_SIZE];  // 2D buffer to store payloads
    uint16_t head;                            // Points to the next write position
    uint16_t tail;                            // Points to the next read position
    uint16_t size;                            // Current number of stored payloads
} CircularBuffer;

typedef struct {
	float temperature;
	float humidity;
	float air_quality;
	int voc;
	int co2;
	int pm;
}air_greg_dev_t;

typedef union {
	uint16_t clear_flags;
	struct {
		uint8_t first_time : 1;
		uint8_t keep_alive : 1;
		uint8_t uart_handle: 1;
		uint8_t isConnected: 1;
		uint8_t ble_conn   : 1;
	}flg;
}CoreFlags_t;


/*Exported variables*/
extern volatile CoreFlags_t device;
extern volatile int sequence_number;
extern volatile int keep_alive;
extern volatile long int pulse1_counter;
extern CircularBuffer cb;
extern air_greg_dev_t air_greg;

/*Exported functions*/
void pulse1_init(void);
void CircularBuffer_Init(CircularBuffer *cb);
bool CircularBuffer_IsEmpty(CircularBuffer *cb);
bool CircularBuffer_IsFull(CircularBuffer *cb);
bool CircularBuffer_Push(CircularBuffer *cb, const char *payload);
bool CircularBuffer_Pop(CircularBuffer *cb, char *payload);

#endif /* DEVICE_H_ */
