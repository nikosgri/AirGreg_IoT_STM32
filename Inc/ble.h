/*
 * ble.h
 *
 *  Created on: Apr 15, 2025
 *      Author: Nikolaos Grigoriadis
 *      Email : n.grigoriadis09@gmail.com
 *      Title : Embedded software engineer
 *      Degree: BSc and MSc in computer science, university of Ioannina
 */

#ifndef BLE_H_
#define BLE_H_

#include <main.h>
#include <wifi.h>
#include <device.h>


typedef enum {
	BLE_OK,
	BLE_ERROR
}ble_status_t;


ble_status_t ble_sync_process(char *credentials);
void ble_split_credentials(const char *credentials, char *ssid, char *pswd);

#endif /* BLE_H_ */
