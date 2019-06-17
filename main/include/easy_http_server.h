/*
 * easy_http_server.h
 *
 *  Created on: 22 Apr 2019
 *      Author: m.bilge
 */

#ifndef MAIN_SRC_INCLUDE_EASY_HTTP_SERVER_H_

#define MAIN_SRC_INCLUDE_EASY_HTTP_SERVER_H_

#include "esp_wifi.h"

void start_config_http();
void start_easy_grow_http();
void set_aps(wifi_ap_record_t aps[], uint16_t apCount);

typedef enum { EASY_MOISTURE, EASY_CONFIG, EASY_REDIRECT } webMode;


#endif /* MAIN_SRC_INCLUDE_EASY_HTTP_SERVER_H_ */

