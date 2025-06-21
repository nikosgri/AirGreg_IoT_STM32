/*
 * ble.c
 *
 *  Created on: Apr 15, 2025
 *      Author: Nikolaos Grigoriadis
 *      Email : n.grigoriadis09@gmail.com
 *      Title : Embedded software engineer
 *      Degree: BSc and MSc in computer science, university of Ioannina
 */



#include <ble.h>


ble_status_t ble_sync_process(char *credentials)
{

	/*Local variables*/
	int result = BLE_ERROR;
	int connection_number = -1;
	char connection_id[30] = {0};

	/*Disable WiFi */
	result = send_command("AT+CWMODE=0", NULL, NULL, "OK", 0, 1000);
	if (result != WIFI_OK) return result;



	result = send_command("AT+SYSSTORE=1", NULL, NULL, "OK", 0, 2000);
	if (result != BLE_OK) {
		LOG_ERR("Failed to set BLE in server mode");
		return result;
	}

	/*BLE Init in Server Mode */
	result = send_command("AT+BLEINIT=2", NULL, NULL, "OK", 0, 2000);
	if (result != BLE_OK) {
		LOG_ERR("Failed to set BLE in server mode");
		return result;
	}


	/*Set BLE Device Name */
	result = send_command("AT+BLENAME=\"AIRGREG\"", NULL, NULL, "OK", 0, 3000);
	if (result != BLE_OK) {
		LOG_ERR("Failed to set device name");
		return result;
	}

//	char adv_data[80];
//	snprintf(adv_data, sizeof(adv_data),
//	         "AT+BLEADVDATA=\"0201060809414952475245471AFF4D0001%02X\"",
//			 device.flg.isConnected); // %02X formats flag as 2-digit hex
//
//	result = send_command(adv_data, NULL, NULL, "OK", 0, 3000);
//		if (result != BLE_OK) {
//			LOG_ERR("Failed to set device name");
//			return result;
//		}

	result = send_command("AT+BLEADVDATA=\"020106080941495247524547\"", NULL, NULL, "OK", 0, 3000);
	if (result != BLE_OK) {
		LOG_ERR("Failed to set device name");
		return result;
	}


	/*Create GATT Service */
	result = send_command("AT+BLEGATTSSRVCRE", NULL, NULL, "OK", 0, 2000);
	if (result != BLE_OK) {
		LOG_ERR("Failed to create GATT service");
		return result;
	}

	/*Start GATT Service */
	result = send_command("AT+BLEGATTSSRVSTART", NULL, NULL, "OK", 0, 2000);
	if (result != BLE_OK) {
		LOG_ERR("Failed to start GATT service");
		return result;
	}

	/*Query bluetooth address*/
	result = send_command("AT+BLEADDR?", "+BLEADDR:", "%s", "OK", 1, 2000, node.url_path);
	if (result != BLE_OK) {
		LOG_ERR("Failed to bluetooth address");
		return result;
	}

	printf("Address: %s\r\n", node.url_path);


	/*Start BLE Advertising */
	result = send_command("AT+BLEADVSTART", "+BLECONN:", "%d, %s", "+BLECONNPARAM:", 2, 820000, &connection_number, connection_id);
	if (result != BLE_OK) {
		LOG_ERR("Failed to start advertising");
		return result;
	}


	if (result == BLE_OK) {
		LOG_INF("Paired successfully");
		printf("[INFO] Clients Address: %s\r\n", connection_id);
		printf("[INFO] Connection number: %d\r\n", connection_number);

	}

	//todo: Add a timeout event!
	while (1) {
	    char *write_start = strstr(uart_receive_buffer, "+WRITE:");
	    if (write_start) {
	        LOG_INF("Paired successfully");

	        int conn_idx, srv_idx, char_idx, len;
	        char *payload;

	        if (sscanf(write_start, "+WRITE:%d,%d,%d,,%d,", &conn_idx, &srv_idx, &char_idx, &len) == 4) {
	            payload = strchr(write_start, ','); // 1st comma
	            payload = strchr(payload + 1, ','); // 2nd comma
	            payload = strchr(payload + 1, ','); // 3rd comma
	            payload = strchr(payload + 1, ','); // 4th comma
	            payload = strchr(payload + 1, ','); // 5th comma, now at real payload

	            if (payload != NULL) {
	                payload++; // Move past the comma
	                strncpy(credentials, payload, len);
	                credentials[len] = '\0'; // Null-terminate
	                printf("Extracted credentials: %s\r\n", credentials);
	                break;
	            }
	        } else {
	            credentials[0] = '\0';
	        }
	    }

	}

	/*Disable BLE*/
	result = send_command("AT+BLEINIT=0", NULL, NULL, "OK", 0, 2000);
	if (result != BLE_OK) {
		LOG_ERR("Failed to disable BLE");
		return result;
	}

	return BLE_OK;
}

/**
 * @brief Splits wifi credentials string into SSID and password parts.
 *
 * This function takes a credentials string formatted as "SSID*PASSWORD"
 * and splits it into two separate buffers: one for the SSID and one for the password.
 *
 * @param credentials  The input credentials string (format: "SSID*PASSWORD").
 * @param ssid         Pointer to the buffer where the extracted SSID will be stored.
 * @param pswd         Pointer to the buffer where the extracted password will be stored.
 *
 * @note Both ssid and pswd buffers must be pre-allocated and large enough.
 * If the separator '*' is not found, both outputs will be set to empty strings.
 */
void ble_split_credentials(const char *credentials, char *ssid, char *pswd)
{
    const char *separator = strchr(credentials, ' '); // Find the position of '*'

    if (separator != NULL) {
        size_t ssid_length = separator - credentials; // Calculate the length of SSID part
        strncpy(ssid, credentials, ssid_length);       // Copy SSID part
        ssid[ssid_length] = '\0';                      // Null-terminate SSID string

        strcpy(pswd, separator + 1);                   // Copy password part (after '*')
    } else {
        // If no '*' found, clear both outputs
        ssid[0] = '\0';
        pswd[0] = '\0';
    }
}

