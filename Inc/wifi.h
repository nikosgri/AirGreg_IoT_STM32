/*
 * wifi.h
 *
 *  Created on: Jul 25, 2024
 *      Author: Nikolaos Grigoriadis
 *      Email : n.grigoriadis09@gmail.com
 *      Title : Embedded software engineer
 *      Degree: BSc and MSc in computer science, university of Ioannina
 */

#ifndef WIFI_H_
#define WIFI_H_

#include <main.h>
#include <timebase.h>
#include <stdarg.h>
#include <stdbool.h>
#include <timebase.h>
#include <rtc.h>
#include <swo.h>
#include <low_level_com.h>
#include <modem.h>


/*Define maximum command size*/
#define MAX_COMMAND_SIZE       50
/*Define maximum UART response size*/
#define MAX_RESPONSE_SIZE      1024
#if defined(MANUAL_SYNC)
/*Name of the local router*/
#define SSID                   "THEOGREG_8"
/*Password of local router*/
#define PSWD                   "mantepsetonvlakentie"
#endif


/*Structure definitions*/
typedef enum WiFi_res
{
    WIFI_OK,
	WIFI_FAIL,
	WIFI_TIMEOUT
}WiFi_res_t;


typedef enum connectionStatus
{
	UNITITIALIZED = 0,    /*Station has not started any Wi-Fi connection.*/
	CONNECTING    = 1,   /*Station has connected to an AP, but does not get an IPv4 address yet.*/
    CONNECTED     = 2,   /*Station has connected to an AP, and got an IPv4 address.*/
	RECONNECTING  = 3,   /*Station is in Wi-Fi connecting or reconnecting state.*/
	DISCONNECTED  = 4,   /*Station is in Wi-Fi disconnected state.*/
}connectionStatus_t;


struct nucleo
{
	connectionStatus_t connection_status;
	int RSSI;
	int32_t temperature_value;
	char board_ip[MAX_COMMAND_SIZE];
	char IMEI_num[MAX_COMMAND_SIZE];
	char url_path[MAX_COMMAND_SIZE];
};

typedef struct nucleo nucleoType;


/*Extern variable declaration*/
extern nucleoType node;

/*Function prototypes*/
void wifi_get_connection_status(void);
WiFi_res_t wifi_register_online();
WiFi_res_t wifi_update_time(rtcType time);
void wifi_frame_format();
int _get_wifi_state(void);


#endif /* WIFI_H_ */
