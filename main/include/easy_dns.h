/*
 * easy_dns.h
 *
 *  Created on: 23 Jun 2019
 *      Author: m.bilge
 */

#ifndef MAIN_INCLUDE_EASY_DNS_H_
#define MAIN_INCLUDE_EASY_DNS_H_
#define ESP_IP "192.168.4.1"

volatile TaskHandle_t dns_handle;

void   startDNS(void);




#endif /* MAIN_INCLUDE_EASY_DNS_H_ */
