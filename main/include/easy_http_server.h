/*
 * easy_http_server.h
 *
 *  Created on: 22 Apr 2019
 *      Author: m.bilge
 */

#ifndef MAIN_SRC_INCLUDE_EASY_HTTP_SERVER_H_

#define MAIN_SRC_INCLUDE_EASY_HTTP_SERVER_H_

#include "esp_wifi.h"

typedef enum { EASY_MOISTURE, EASY_CONFIG, EASY_DOCUMENTATION } webMode;

void start_http(webMode webMode);
void set_aps(wifi_ap_record_t aps[], uint16_t apCount);
const char* getMoistureOutputString(uint16_t level);

#endif /* MAIN_SRC_INCLUDE_EASY_HTTP_SERVER_H_ */

