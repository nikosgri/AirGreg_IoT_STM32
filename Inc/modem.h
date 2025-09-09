/*
 * modem.h
 *
 *  Created on: Jul 13, 2025
 *      Author: Nikolaos Grigoriadis
 *      Email : n.grigoriadis09@gmail.com
 *      Title : Embedded software engineer
 *      Degree: BSc and MSc in computer science, university of Ioannina
 */

#ifndef MODEM_H_
#define MODEM_H_

#include "main.h"
#include "wifi.h"
#include <device.h>
#include <stdarg.h>
#include <stdbool.h>
#include <timebase.h>
#include <rtc.h>


typedef enum modem_status
{
	MODEM_OK = 0,
	MODEM_ERROR,
	MODEM_TIMEOUT
}modem_status_t;


modem_status_t modem_open_connection(const char * server_ip, int port_number);
modem_status_t modem_close_connection(void);
modem_status_t modem_send_udp(void);
modem_status_t modem_receive_data(char * response);
modem_status_t modem_power_down(void);
modem_status_t modem_is_accessible(void);
modem_status_t modem_get_imei(void);
modem_status_t modem_get_rssi(void);
int modem_get_sleep_state(void);
modem_status_t send_command(const char *command, const char *exp, const char *exp_parse, const char *exp_end, uint32_t num_of_exp, uint32_t delay, ...);


#endif /* MODEM_H_ */
