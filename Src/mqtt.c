/*
 * mqtt.c
 *
 *  Created on: May 20, 2025
 *      Author: Nikolaos Grigoriadis
 *      Email : n.grigoriadis09@gmail.com
 *      Title : Embedded software engineer
 *      Degree: BSc and MSc in computer science, university of Ioannina
 */


#include <mqtt.h>


mqtt_status_t MQTT;


/**
 * @brief Cleans (closes) the current MQTT connection using AT+MQTTCLEAN.
 *
 * Sends an AT command to the ESP32 to close the active MQTT connection
 * and reset the link for reuse.
 *
 * @param link_id The link ID to close. Only 0 is supported.
 * @return 0 on success, -1 on error or if command fails.
 */
int mqtt_close_connection(int link_id)
{
	/* Local variable */
	char cmd[15]={0};
	int linkid = -1;
	int res = -1;

	/* Prepare the command */
	snprintf(cmd, sizeof(cmd), "AT+MQTTCLEAN=%d", link_id);

	/* Close the MQTT connection, and release the resource */
	res = send_command_wait_result(cmd, "+MQTTDISCONNECTED:", "%d", "OK", 0, 3000, &linkid);
	if (res != LL_OK)
	{
		LOG_ERR("mqtt_close_connection() failed with return code: %d", res);
		return res;
	}

	/* Success */
	return 0;
}


/**
 * @brief Unsubscribe from a topic using AT+MQTTUNSUB.
 *
 * Sends an unsubscribe request for the specified topic to the MQTT broker.
 *
 * @param link_id The link ID to use. Only 0 is supported.
 * @param topic   The topic string to unsubscribe from.
 * @return 0 on success, -1 on failure or if the topic cannot be unsubscribed.
 */
int mqtt_topic_unsub(int link_id, char *topic)
{
	/* Local variable */
	char cmd[40]={0};
	int res = -1;

	/* Prepare the command */
	snprintf(cmd, sizeof(cmd), "AT+MQTTUNSUB=%d,%s", link_id, topic);

	/* Unsubscribe the client from defined topic */
	res = send_command_wait_result(cmd, "OK", NULL, NULL, 0, 1000);
	if (res != LL_OK)
	{
		LOG_ERR("mqtt_topic_unsub() failed with return code: %d", res);
		return res;
	}

	/* Check if there is an issue with the unsubscribe */
	if (strstr(uart_receive_buffer, "NO UNSUBSCRIBE"))
	{
		LOG_WRN("Failed to unsubscribe from topic: %s", topic);
		return -1;
	}

	/* Success */
	return 0;
}


/**
 * @brief Connects the ESP32 to an MQTT broker using AT+MQTTCONN.
 *
 * This function constructs and sends the AT command to establish an MQTT connection.
 * If reconnect is set to 0, it first performs a clean-up using mqtt_clean_connect().
 *
 * @param link_id Link ID for the MQTT connection. Only 0 is supported.
 * @param server  MQTT broker domain or IP address (maximum 128 bytes).
 * @param port    Port number of the MQTT broker (0–65535).
 * @param reconnect Reconnect behavior:
 *        - 0: No automatic reconnection. Requires cleanup before reuse.
 *        - 1: Automatic reconnection enabled. Uses more system resources.
 *
 * @return 0 on success,
 *         -1 on validation error or failure to connect.
 */
int mqtt_connect(int link_id, char *server, int port, int reconnect)
{
	/* Local variable */
	int linkid = -1;
	char cmd[MAX_AT_CMD_LENGTH]={0};
	int res = -1;


	/* Validate input */
    if (link_id != 0)
    {
        LOG_ERR("Only LinkID 0 is supported.");
        return -1;
    }

    if (server == NULL || strlen(server) > 128)
    {
        LOG_WRN("Host must be non-null and not greater than 128 bytes.");
        return -1;
    }


	if (port < 0 || port > 65535)
	{
		LOG_ERR("Invalid port number.");
		return -1;
	}

    if (reconnect != 0 && reconnect != 1)
    {
        LOG_ERR("Reconnect must be 0 or 1.");
        return -1;
    }

	if (reconnect==0)
	{
		/* Clean every existing MQTT connection */
		mqtt_clean_connect(link_id);
	}


	/* Prepare command */
    snprintf(cmd, sizeof(cmd),
             "AT+MQTTCONN=%d,\"%s\",%d,%d",
             link_id, server, port, reconnect);

	/* Connect to a MQTT broker */
	res = send_command_wait_result(cmd, "+MQTTCONNECTED:", "%d,%d,%s,%d,%s,%d", "OK", 0, 15000,
			&linkid, &MQTT.scheme, MQTT.server, &MQTT.port, MQTT.path);
	if (res != LL_OK)
	{
		LOG_ERR("mqtt_connect() failed with return code: %d", res);
		return res;
	}

	/* Success */
	return 0;
}


/**
 * @brief Subscribe to a topic using AT+MQTTSUB.
 *
 * Sends a subscribe request for the specified topic and QoS level.
 *
 * @param link_id The MQTT link ID (only 0 is supported).
 * @param topic   Topic string to subscribe to.
 * @param qos     Quality of Service level (0, 1, or 2).
 * @return 0 on success, -1 on validation error or if subscription fails.
 */
int mqtt_subscribe(int link_id, char *topic, int qos)
{
	/* Local variable */
	char cmd[MAX_AT_CMD_LENGTH]={0};
	int res = -1;

	/* Validate input */
    if (link_id != 0)
    {
        LOG_ERR("Only LinkID 0 is supported.");
        return -1;
    }

    if (topic == NULL)
    {
    	LOG_ERR("No topic provided.");
    	return -1;
    }

    if (qos<0 || qos>2)
    {
    	LOG_WRN("Invalid Qos option.");
    	return -1;
    }

    /* Prepare command */
    snprintf(cmd, sizeof(cmd), "AT+MQTTSUB=%d,\"%s\",%d", link_id, topic, qos);

    /* Subscribe to topic */
    res = send_command_wait_result(cmd, "OK", NULL, NULL, 0, 2000);
    if (res != LL_OK)
    {
    	if (strstr(uart_receive_buffer, "ALREADY SUBSCRIBE"))
    	{
    		LOG_INF("Already susbscribed to topic: %s", topic);
    		return -1;
    	}

		LOG_ERR("mqtt_subscribe() failed with return code: %d", res);
		return -1;
    }


    /* Success */
    return 0;
}

/**
 * @brief Publishes a message to a topic using AT+MQTTPUB.
 *
 * Publishes a message to the broker the ESP32 is currently connected to.
 *
 * @param link_id The MQTT link ID (only 0 is supported).
 * @param topic   Topic to publish to (max 128 bytes).
 * @param data    Payload to publish.
 * @param qos     Quality of Service level (0, 1, or 2).
 * @param retain  Retain flag (0 = no retain, 1 = retain).
 * @return 0 on success, -1 on validation error or send failure.
 */
int mqtt_publish(int link_id, char *topic, char *data, int qos, int retain)
{
	/* Local variable */
	char cmd[MAX_AT_CMD_LENGTH]={0};
	int res = -1;

	/* Validate input */
    if (link_id != 0)
    {
        LOG_ERR("Only LinkID 0 is supported.");
        return -1;
    }

	if (topic == NULL || strlen(topic) > MAX_TOKEN_LENGTH)
	{
		LOG_WRN("Topic should not be null and greater than 128 bytes.");
		return -1;
	}

	/* Check Qos number */
	if (qos < 0 || qos > 2)
	{
		LOG_WRN("Invalid Qos number.");
		return -1;
	}

	/* Prepare the command */
	int size_of_cmd = snprintf(cmd, sizeof(cmd), "AT+MQTTPUB=%d,\"%s\",\"%s\",%d,%d",
			link_id, topic, data, qos, retain);

	if (size_of_cmd > MAX_AT_CMD_LENGTH)
	{
		LOG_WRN("Command is too long, cannot procced! Try using the <AT+MQTTPUBRAW> instead!");
		return -1;
	}

	/* Publish the message to the MQTT broker that you are connected to */
	res = send_command_wait_result(cmd, "OK", NULL, NULL, 0, 10000);
	if (res != LL_OK)
	{
		LOG_ERR("mqtt_publish() failed with return code: %d", res);
		return -1;
	}

	/* Success */
	return 0;
}


/**
 * @brief Publish a long MQTT payload using AT+MQTTPUB with data prompt.
 *
 * This function is used for publishing messages larger than what fits in a single AT command.
 * It first sends a publish command with the size, waits for the `>` prompt, then sends the payload.
 *
 * @param link_id The MQTT link ID (only 0 is supported).
 * @param topic   Topic to publish to (max 128 bytes).
 * @param data    Pointer to the payload data.
 * @param length  Length of the payload.
 * @param qos     Quality of Service level (0, 1, or 2).
 * @param retain  Retain flag (0 = no retain, 1 = retain).
 * @return 0 on success, -1 on validation or publish failure.
 */
int mqtt_publish_long_data(int link_id, char *topic, char *data, int length, int qos, int retain)
{
	/* Local variable */
	char status[10] = {0};
	char cmd[MAX_AT_CMD_LENGTH]={0};
	int res = -1;

	/* Validate input */
    if (link_id != 0)
    {
        LOG_ERR("Only LinkID 0 is supported.");
        return -1;
    }

	if (topic == NULL || strlen(topic) > MAX_TOKEN_LENGTH)
	{
		LOG_WRN("Topic should not be null and greater than 128 bytes.");
		return -1;
	}

	/* Check Qos number */
	if (qos < 0 || qos > 2)
	{
		LOG_WRN("Invalid Qos number.");
		return -1;
	}

	/* Prepare command */
	snprintf(cmd, sizeof(cmd), "AT+MQTTPUB=%d,\"%s\",%d,%d,%d",
				link_id, topic, length, qos, retain);

	/* Indicate the module for long data message */
	res = send_command_wait_result(cmd, ">", NULL, NULL, 0, 2000);
	if (res != LL_OK)
	{
		LOG_ERR("mqtt_publish_long_data() failed with return code: %d", res);
		return -1;
	}

	/* Send the long payload */
	res = send_command_wait_result(data, "+MQTTPUB:", "%s", NULL, 1, 8000, status);
	if (res != LL_OK)
	{
		LOG_ERR("mqtt_publish_long_data() failed with return code: %d", res);
		return -1;
	}

	if (strncmp(status, "FAIL", 4) == 0)
	{
		return -1;
	}


	return 0;
}

/**
 * @brief Cleans up the MQTT connection using AT+MQTTCLEAN.
 *
 * Sends the clean command to terminate and reset the current MQTT link.
 *
 * @param link_id The link ID to clean. Only 0 is supported.
 * @return 0 on success, -1 on error or failure to clean.
 */
int mqtt_clean_connect(int link_id)
{
	/* Local variable */
	char cmd[MAX_AT_CMD_LENGTH]={0};
	int res = -1;

	/* Validate input */
    if (link_id != 0)
    {
        LOG_ERR("Only LinkID 0 is supported.");
        return -1;
    }

    /* Prepare command */
    snprintf(cmd, sizeof(cmd), "AT+MQTTCLEAN=%d", link_id);


	/* Clean MQTT connection */
	res = send_command_wait_result(cmd, "OK", NULL, NULL, 0, 1000);
	if (res != LL_OK)
	{
		LOG_ERR("mqtt_clean_connect() failed with return code: %d", res);
		return -1;
	}

	return 0;
}


/**
 * @brief Configure MQTT user credentials and connection parameters via AT command.
 *
 * This function builds and sends the AT+MQTTUSERCFG command to configure the MQTT client.
 *
 * @param link_id Connection link ID. Only 0 is supported.
 * @param scheme MQTT connection scheme (TCP, TLS, WebSocket variants).
 * @param client_id MQTT client ID (max 256 bytes).
 * @param username Username for MQTT broker authentication (max 64 bytes).
 * @param password Password for MQTT broker authentication (max 64 bytes).
 * @param path Resource path (max 32 bytes).
 *
 * @return 0 on success, -1 on failure, or low level errors.
 */
int mqtt_user_config(int link_id, mqtt_scheme_t scheme, const char *client_id, const char *username,
                     const char *password, const char *path)
{
	/* Local variable */
	char cmd[MAX_AT_CMD_LENGTH]={0};
	int res = -1;

	/* Validate input */
    if (link_id != 0)
    {
        LOG_ERR("Only LinkID 0 is supported.");
        return -1;
    }

    if (scheme < MQTT_SCHEME_TCP || scheme > MQTT_SCHEME_WSS_VERIFY_SERVER_CLIENT_CERT)
    {
    	LOG_WRN("Invalid MQTT scheme.");
    	return -1;
    }

    if (username == NULL || password == NULL)
    {
    	LOG_WRN("Invalid username or password, should not be null.");
    	return -1;
    }

	/* Prepare command */
    int size_of_cmd = snprintf(cmd, sizeof(cmd),
             "AT+MQTTUSERCFG=%d,%d,\"%s\",\"%s\",\"%s\",%d,%d,\"%s\"",
             link_id,
             scheme,
             client_id,
             username,
             password,
             0,
             0,
             path);

    if (size_of_cmd > MAX_AT_CMD_LENGTH)
    {
		LOG_WRN("Command is too long, cannot procced!");
		return -1;
    }

    /* Set user configuration parameters */
    res = send_command_wait_result(cmd, "OK", NULL, NULL, 0, 1000);
    if (res != LL_OK)
    {
		LOG_ERR("mqtt_user_config() failed with return code: %d", res);
		return -1;
    }

    /* Success */
    return 0;
}


/**
 * @brief Set a long MQTT client ID using the AT+MQTTLONGCLIENTID command.
 *
 * This function sends the AT+MQTTLONGCLIENTID command to prepare the modem for receiving
 * a long MQTT client ID (up to 1024 bytes). After the initial command, it sends the actual
 * client ID string, and waits for confirmation.
 *
 * This should be used after calling mqtt_user_config(), and is especially useful when the
 * client ID exceeds the size limit of AT+MQTTUSERCFG.
 *
 * @param link_id   MQTT connection link ID. Only 0 is supported.
 * @param client_id Pointer to the client ID string to be sent.
 * @param length    Length of the client ID. Must match the actual string length.
 *                  Range: [1, 1024].
 *
 * @return 0 on success, -1 on validation error or modem communication failure.
 */
int mqtt_set_long_client_id(int link_id, const char *client_id, size_t length)
{
	/* Local variable */
	char cmd[MAX_AT_CMD_LENGTH]={0};
	int res = -1;

	/* Validate input */
    if (link_id != 0)
    {
        LOG_ERR("Only LinkID 0 is supported.");
        return -1;
    }

    /* Prepare the command */
    snprintf(cmd, sizeof(cmd), "AT+MQTTLONGCLIENTID=%d,%d", link_id, length);

    /* Indicate that you want to set long client id name */
    res = send_command_wait_result(cmd, ">", NULL, NULL, 0, 2000);
    if (res != LL_OK)
    {
		LOG_ERR("mqtt_set_long_client_id() failed with return code: %d", res);
		return -1;
    }

    /* Send the client id */
    res = send_command_wait_result(client_id, "OK", NULL, NULL, 0, 2000);
    if (res != LL_OK)
    {
		LOG_ERR("mqtt_set_long_client_id() failed with return code: %d", res);
		return -1;
    }

    /* Success */
    return 0;
}


/**
 * @brief Configure MQTT connection parameters (e.g., keep-alive, clean session, LWT).
 *
 * Uses AT+MQTTCONNCFG to configure the keep-alive timer, clean session flag, and
 * last will and testament (LWT) parameters.
 *
 * @param link_id       MQTT connection link ID. Only 0 is supported.
 * @param keep_alive    Keep-alive interval in seconds (0–7200).
 * @param clean_session Clean session flag (0 or 1).
 * @param lwt_topic     Last Will and Testament topic string.
 * @param lwt_msg       LWT message payload.
 * @param lwt_qos       QoS level for LWT (0–2).
 * @param lwt_retain    Retain flag for LWT (0 or 1).
 * @return 0 on success, -1 on validation or configuration error.
 */
int mqtt_set_configuration_connection(int link_id, int keep_alive, int clean_session, char * lwt_topic,
		char * lwt_msg, int lwt_qos, int lwt_retain)
{
	/* Local variable */
	char cmd[MAX_AT_CMD_LENGTH]={0};
	int res = -1;

	/* Validate input */
    if (link_id != 0)
    {
        LOG_ERR("Only LinkID 0 is supported.");
        return -1;
    }

    if (keep_alive < 0 || keep_alive > 7200)
    {
    	LOG_WRN("Invalid Keep alive configuration. Choose between [0,7200].");
    	return -1;
    }

    if (clean_session != 0 || clean_session != 1)
    {
    	LOG_WRN("Invalid <clean_session>. Select 0 for disabling it, or 1 for enabling it.");
    	return -1;
    }

    if (lwt_qos < 0 || lwt_qos > 2)
    {
    	LOG_ERR("Invalid Qos number.");
    	return -1;
    }

    if (lwt_retain != 0 || lwt_retain != 1)
    {
    	LOG_WRN("Invalid <lwt_retain> number. Select one between (0-1).");
    	return -1;
    }

    if (strlen(lwt_msg) > MAX_TOKEN_LENGTH)
    {
    	LOG_WRN("Invalid LWT message length, should be less than 128 bytes.");
    	return -1;
    }

    if (strlen(lwt_topic) > MAX_TOKEN_LENGTH)
    {
    	LOG_WRN("Invalid LWT topic length, should be less than 128 bytes.");
    	return -1;
    }

    /* Prepare the command */
    snprintf(cmd, sizeof(cmd), "AT+MQTTCONNCFG=%d,%d,%d,\"%s\",\"%s\",%d,%d",
    		link_id, keep_alive, clean_session, lwt_topic, lwt_msg, lwt_qos, lwt_retain);

    /* Set connection configurations */
    res = send_command_wait_result(cmd, "OK", NULL, NULL, 0, 1000);
    if (res != LL_OK)
    {
		LOG_ERR("mqtt_set_configuration_connection() failed with return code: %d", res);
		return -1;
    }

    /* Success */
    return 0;
}


/**
 * @brief Set Server Name Indication (SNI) for TLS-based MQTT connections.
 *
 * Sends AT+MQTTSNI command to configure the SNI value, required by some brokers
 * for TLS handshakes.
 *
 * @param link_id MQTT connection link ID. Only 0 is supported.
 * @param sni     Server Name Indication string.
 * @return 0 on success, -1 on validation or AT command error.
 */
int mqtt_set_server_name_indication(int link_id, char *sni)
{
	/* Local variable */
	char cmd[MAX_AT_CMD_LENGTH]={0};
	int res = -1;

	/* Validate input */
    if (link_id != 0)
    {
        LOG_ERR("Only LinkID 0 is supported.");
        return -1;
    }

    if (sni == NULL)
    {
    	LOG_ERR("Invalid Server name Indication, should not be null.");
    	return -1;
    }

    /* Prepare the command */
    snprintf(cmd, sizeof(cmd), "AT+MQTTSNI=%d,\"%s\"", link_id, sni);

    /* Set server name indication */
    res = send_command_wait_result(cmd, "OK", NULL, NULL, 0, 1000);
    if (res != LL_OK)
    {
		LOG_ERR("mqtt_set_server_name_indication() failed with return code: %d", res);
		return -1;
    }

    /* Success */
    return 0;
}
