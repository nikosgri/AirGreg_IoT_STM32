/*
 * wifi.c
 *
 *  Created on: Jul 25, 2024
 *      Author: Nikolaos Grigoriadis
 *      Email : n.grigoriadis09@gmail.com
 *      Title : Embedded software engineer
 *      Degree: BSc and MSc in computer science, university of Ioannina
 */


#include <wifi.h>
#include <ctype.h>
#include <adc.h>
#include <device.h>


/*Function prototypes*/
static uint32_t _extract_month(char *month);

/*Global variables*/
nucleoType node;     // Variable which contains details about nucleo information.
int mux_mode;        // Variable that checks the UDP receive mode.


/**
 * @brief Initialize WiFi module and connect to router.
 *
 * @retval WIFI_OK on success, non-zero error code on failure.
 */
WiFi_res_t wifi_register_online(void)
{
    /*Local variable declaration*/
    char command[256] = {0};
    int result_code = -1;

    /*Check if the WiFi device is accessible*/
    result_code = modem_is_accessible();
    if (result_code != WIFI_OK)
    {
        LOG_WRN("WiFi module isn't accessible");
    }

    /*Check the connection status*/
    if (device.flg.ble_conn == 0 || node.connection_status != CONNECTED || device.flg.first_time)
    {
    	LOG_INF("Start Connecting Online...");

        /*Initialize WiFi driver*/
        result_code = send_command("AT+CWINIT=1", NULL, NULL, "OK", 0, 1000);
        if (result_code != WIFI_OK)
        {
             return result_code;
        }

        /*Set the wifi mode to station mode*/
        result_code = send_command("AT+CWMODE=1", NULL, NULL, "OK", 0, 1000);
        if (result_code != WIFI_OK)
        {
            return result_code;
        }

#if defined(MANUAL_SYNC)
        /*Connect to the local router*/
        snprintf(command, sizeof(command), "AT+CWJAP=\"%s\",\"%s\"", SSID, PSWD);
        result_code = send_command(command, NULL, NULL, "OK", 0, 5000);
        if (result_code != WIFI_OK)
        {
            return result_code;
        }

#elif defined(BLE_SYNC)
        /*Connect to the local router*/
        snprintf(command, sizeof(command), "AT+CWJAP=\"%s\",\"%s\"", ssid, pswd);
        result_code = send_command(command, NULL, NULL, "OK", 0, 9000);
        if (result_code != WIFI_OK)
        {
            return result_code;
        }
#endif

        /*The ESP32 station tries to reconnect to AP at the interval of one second for 100 times*/
        result_code = send_command("AT+CWRECONNCFG=1,100", NULL, NULL, "OK", 0, 1000);
        if (result_code != WIFI_OK)
        {
            return result_code;
        }
    }

    /*Check connection mode*/
    result_code = send_command("AT+CIPMUX?", "+CIPMUX:", "%d", "OK", 1, 1000, &mux_mode);
    if (result_code != WIFI_OK)
    {
        return result_code;
    }

    if ((mux_mode || device.flg.first_time) && mux_mode != 0)
    {
        /*Change to single connection*/
        result_code = send_command("AT+CIPMUX=0", "OK", NULL, "OK", 0, 4000);
    	if (result_code != WIFI_OK)
        {
            return result_code;
        }
    }

    /*Enable active receiving mode*/
    result_code = send_command("AT+CIPRECVTYPE=1", "OK", NULL, "OK", 0, 2000);
    if (result_code != 0)
    {
        return -1;
    }

    /*Query the IP Address of an ESP32 Station*/
    result_code = send_command("AT+CIPSTA?", "+CIPSTA:ip:", "%s", "OK", 1, 1000, node.board_ip);
    if (result_code != WIFI_OK)
    {
        return result_code;
    }

    LOG_INF("BOARDS IP ADDRESS...");
    LOG_INF("%s", node.board_ip);
    device.flg.isConnected = 1;
    printf("%c%c%c%c", RETURN, NEWLINE, RETURN, NEWLINE);


    /*show the remote host and port in “+IPD” and “+CIPRECVDATA” messages.*/
    result_code = send_command("AT+CIPDINFO=1", NULL, NULL, "OK", 0, 1000);
    if (result_code != WIFI_OK)
    {
        return result_code;
    }

    return result_code;
}


/**
 * @brief Update RTC time from NTP server.
 *
 * @param time RTC structure to update.
 * @retval WIFI_OK on success, non-zero error code on failure.
 */
WiFi_res_t wifi_update_time(rtcType time)
{
    /*Local variable declaration*/
    WiFi_res_t result_code = -1;
    char month[4]={0},date[4]={0};
    int num, hour, min, sec, year=1970;
    uint32_t start_time = 0;

    /*Set the desired time zone and the server to which we will connect to*/
    result_code = send_command("AT+CIPSNTPCFG=1,0,\"time.google.com\"", NULL, NULL, "+TIME_UPDATED", 0, 20000);

    /*Check the result code*/
    if (result_code != 0)
    {
        return result_code;
    }

    start_time = get_tick();
    while (((get_tick() - start_time) < 3000)  && year == 1970)
    {
        /*Read time from NTP server to update the RTC clock*/
        result_code = send_command("AT+CIPSNTPTIME?", "+CIPSNTPTIME:", "%s %s %d %d:%d:%d %d", "OK", 7, 2000, date, month, &num, &hour, &min, &sec, &year);
    }

    /*Check the result code*/
    if (result_code != 0)
    {
        return result_code;
    }

    int i = (_extract_month(month)+1);
    /*Update RTC parameters*/
    time.hour   = _RTC_convert_bin2bcd(hour);
    time.minute = _RTC_convert_bin2bcd(min);
    time.second = _RTC_convert_bin2bcd(sec);
    time.day    = _RTC_convert_bin2bcd(num);
    time.month  = _RTC_convert_bin2bcd(i);
    time.week   = 0x02;
    time.year   = _RTC_convert_bin2bcd(year-2000);

    /*Update RTC*/
    rtc_init(time);

    /*Return the result to FSM*/
    return result_code;
}


/**
 * @brief Get current WiFi connection status.
 */
void wifi_get_connection_status(void)
{
    /*Local variables*/
    int result_code = -1;
    char ssid[10];

    /*Take the IMEI number of the WiFi modem*/
    result_code = send_command("AT+CWSTATE?", "+CWSTATE:", "%d,%s", "OK", 2, 2000, &node.connection_status, ssid);

    if (result_code != 0)
    {
        LOG_WRN("Something went wrong while querying the WiFi connection status");
    }

}


/**
 * @brief Format and enqueue UDP JSON payload.
 */
void wifi_frame_format(void)
{
	/*Local variables*/
    char payload[500] ={0};

    /*Retrieve RSSI*/
    modem_get_rssi();

    /*Create the UDP frame (JSON)*/
    snprintf(payload, sizeof(payload), "{\"1\":%d, \"2\":%s, \"3\":%d, \"4\":%lu, \"5\":%d, \"6\":%ld, \"7\":%ld}",
    		sequence_number, node.IMEI_num, node.RSSI, RTC_get_timestamp(), device.flg.keep_alive, pulse1_counter, read_internal_temp(adc1_read()));

    /*Push data into the circular buffer*/
    CircularBuffer_Push(&cb, payload);
}


/**
 * @brief Extract month index from abbreviation.
 *
 * @param month Abbreviated month name (e.g., "Jan").
 * @retval Month index (0–11), -1 if invalid.
 */
static uint32_t _extract_month(char *month)
{
    /*Array with abbreviated month names*/
    const char *months[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
     };

     /*Loop through the months array and compare the string with each month*/
     for (uint32_t i = 0; i < 12; i++)
     {
	      /*Check if the input string match with one of the months*/
	      if (strcmp(months[i], month) == 0)
	      {
		       /*Return the index if match is found*/
		       return i;
	      }
     }

     /*Return -1 if no match is found*/
     return -1;
}


