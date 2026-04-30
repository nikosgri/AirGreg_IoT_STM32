/*
 * https.h
 *
 *  Created on: May 24, 2025
 *      Author: Nikolaos Grigoriadis
 *      Email : n.grigoriadis09@gmail.com
 *      Title : Embedded software engineer
 *      Degree: BSc and MSc in computer science, university of Ioannina
 */

#ifndef HTTPS_H_
#define HTTPS_H_

#include "main.h"
#include "ctype.h"


#define DATABASE_HEADERS "Content-Type: application/json"

typedef enum {
	HTTPS_OK,
	HTTPS_FAIL
}https_status;

typedef enum {
    HTTP_METHOD_HEAD = 1,
    HTTP_METHOD_GET,
    HTTP_METHOD_POST,
    HTTP_METHOD_PUT,
    HTTP_METHOD_DELETE
} http_method_t;

typedef enum {
    CONTENT_TYPE_FORM_URLENCODED = 0,
    CONTENT_TYPE_JSON,
    CONTENT_TYPE_MULTIPART,
    CONTENT_TYPE_XML
} http_content_type_t;

typedef enum {
    HTTP_TRANSPORT_TCP = 1,
    HTTP_TRANSPORT_SSL
} http_transport_t;



https_status https_set_url(const char *url);
https_status https_set_headers(const char *headers);
https_status https_post(const char *url, const char *data);
https_status https_client_request();
https_status https_put(const char *url, const char *data);
void https_url_encode_colons(char *mac, char *encoded, size_t encoded_size);
https_status https_update_firebase(void);

#endif /* HTTPS_H_ */
