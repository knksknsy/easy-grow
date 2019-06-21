/*
 * easy_wifimanager.h
 *
 *  Created on: 24 May 2019
 *      Author: m.bilge
 */

#ifndef MAIN_INCLUDE_EASY_WIFI_MANAGER_H_
#define MAIN_INCLUDE_EASY_WIFI_MANAGER_H_


void sta_wifi_init(char ssid[32], char pwd[64]);
void ap_wifi_init();
void general_wifi_init();
void reset_wifi_credentials();

#endif /* MAIN_INCLUDE_EASY_WIFI_MANAGER_H_ */
