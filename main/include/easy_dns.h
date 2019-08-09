/*
 * easy_dns.h
 *
 *  Created on: 23 Jun 2019
 *      Author: m.bilge
 */

#ifndef MAIN_INCLUDE_EASY_DNS_H_
#define MAIN_INCLUDE_EASY_DNS_H_


volatile int dns_task_initialized;
volatile TaskHandle_t dns_handle;

typedef struct __attribute__ ((packed)) {
	uint16_t id;
	uint8_t flags;
	uint8_t rcode;
	uint16_t qdcount;
	uint16_t ancount;
	uint16_t nscount;
	uint16_t arcount;
} DnsHeader;


typedef struct __attribute__ ((packed)) {
	//before: label
	uint16_t type;
	uint16_t class;
} DnsQuestionFooter;


typedef struct __attribute__ ((packed)) {
	//before: label
	uint16_t type;
	uint16_t class;
	uint32_t ttl;
	uint16_t rdlength;
	//after: rdata
} DnsResourceFooter;


#define FLAG_QR (1<<7)
#define FLAG_TC (1<<1)

/*
 * Genutze DNS RECORD Typen
 */
#define RECORD_TYPE_A  1
#define RECORD_TYPE_NS 2
#define RECORD_TYPE_URI 256

#define DNS_CLASS_IN 1
#define DNS_CLASS_ANY 255
#define DNS_CLASS_URI 256

#define LENGTH_DNS 512


/*
 * Starten des DNS Servers mit der Weiterleitung
 */
void startDNS(void);




#endif /* MAIN_INCLUDE_EASY_DNS_H_ */
