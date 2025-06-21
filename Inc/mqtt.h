/*
 * mqtt.h
 *
 *  Created on: May 20, 2025
 *      Author: grego
 */

#ifndef MQTT_H_
#define MQTT_H_

#include <main.h>
#include <low_level_com.h>

#define MAX_AT_CMD_LENGTH 256
#define MAX_TOKEN_LENGTH 128
#define API_KEY	"AIzaSyBe88SldHTnA2bkSOuWKNXMrgWmes7WRDs"
#define DATABASE_URL "https://smart-indoor-system-default-rtdb.europe-west1.firebasedatabase.app/"

/**
 * @brief MQTT connection scheme enumeration for ESP32 AT commands.
 */
typedef enum {
    MQTT_SCHEME_TCP = 1,
    MQTT_SCHEME_TLS_NO_VERIFY,
    MQTT_SCHEME_TLS_VERIFY_SERVER,
    MQTT_SCHEME_TLS_CLIENT_CERT,
    MQTT_SCHEME_TLS_VERIFY_SERVER_CLIENT_CERT,
    MQTT_SCHEME_WS_TCP,
    MQTT_SCHEME_WSS_NO_VERIFY,
    MQTT_SCHEME_WSS_VERIFY_SERVER,
    MQTT_SCHEME_WSS_CLIENT_CERT,
    MQTT_SCHEME_WSS_VERIFY_SERVER_CLIENT_CERT
} mqtt_scheme_t;


typedef struct {
    int port;
    mqtt_scheme_t scheme;
    char *server;
    char *path;
} mqtt_status_t;

extern mqtt_status_t MQTT;

int mqtt_close_connection(int link_id);
int mqtt_topic_unsub(int link_id, char *topic);
int mqtt_connect(int link_id, char *server, int port, int reconnect);
char *mqtt_get_broker(int link_id);
int mqtt_publish(int link_id, char *topic, char *data, int qos, int retain);
int mqtt_publish_long_data(int link_id, char *topic, char *data, int length, int qos, int retain);
int mqtt_clean_connect(int link_id);
int mqtt_set_configuration_connection(int link_id, int keep_alive, int clean_session, char * lwt_topic, char * lwt_msg, int lwt_qos, int lwt_retain);
int mqtt_subscribe(int link_id, char *topic, int qos);
int mqtt_user_config(int link_id, mqtt_scheme_t scheme, const char *client_id, const char *username,
                     const char *password, const char *path);
int mqtt_set_long_client_id(int link_id, const char *client_id, size_t length);
int mqtt_set_server_name_indication(int link_id, char *sni);

#endif /* MQTT_H_ */
