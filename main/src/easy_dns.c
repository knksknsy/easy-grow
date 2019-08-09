/*
 * easy_dns.c
 *
 *  Created on: 23 Jun 2019
 *      Author: m.bilge
 *
 *
 * Modified version of:
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Jeroen Domburg <jeroen@spritesmods.com> wrote this file. As long as you retain
 * this notice you can do whatever you want with this stuff. If we meet some day,
 * and you think this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 *
 *
 *
 *
 */

#include "FreeRTOS.h"
#include "task.h"

#include "queue.h"
#include <easy_util.h>
#include "lwip/sockets.h"
#include "lwip/err.h"
#include "tcpip_adapter.h"
#include "esp_libc.h"
#include "string.h"
#include "lwip/ip4_addr.h"
#include <esp_wifi.h>
#include <esp_system.h>
#include "lwip/opt.h"
#include "lwip/err.h"
#include "lwip/ip_addr.h"
#include "lwip/mem.h"
#include <stdlib.h>
#include <esp_log.h>
#include <easy_dns.h>

#define TAG "EASY_DNS"

static uint16_t local_ntohs(uint16_t *in) {
	char *p = (char*) in;
	return ((p[0] << 8) & 0xff00) | (p[1] & 0xff);
}

/*
 * Parsen des DNS Format in eine C String
 */
static char* labelToStr(char *packet, char *labelPtr, int packetSz, char *res,
		int resMaxLen) {
	int i, j, k;
	char *endPtr = NULL;
	i = 0;
	do {
		if ((*labelPtr & 0xC0) == 0) {
			j = *labelPtr++;
			if (i < resMaxLen && i != 0)
				res[i++] = '.';
			for (k = 0; k < j; k++) {
				if ((labelPtr - packet) > packetSz)
					return NULL;
				if (i < resMaxLen)
					res[i++] = *labelPtr++;
			}
		} else if ((*labelPtr & 0xC0) == 0xC0) {
			endPtr = labelPtr + 2;
			int offset = local_ntohs(((uint16_t *) labelPtr)) & 0x3FFF;
			if (offset > packetSz)
				return NULL;
			labelPtr = &packet[offset];
		}
		if ((labelPtr - packet) > packetSz)
			return NULL;
	} while (*labelPtr != 0);
	res[i] = 0;ewwwe
	if (endPtr == NULL)
		endPtr = labelPtr + 1;
	return endPtr;
}

/*
 * Parsen des Strings in den DNS Format
 */
static char *strToLabel(char *str, char *label, int maxLen) {
	char *len = label;
	char *p = label + 1;
	while (1) {
		if (*str == '.' || *str == 0) {
			*len = ((p - len) - 1);
			len = p;
			p++;
			if (*str == 0)
				break;
			str++;
		} else {
			*p++ = *str++;
		}
	}
	*len = 0;
	return p;
}

#define DNS_SERVER_PORT   53

int sock_fd;
/*
 * Erhalt einer DNS Nachricht
 */
static void dnsRecv(struct sockaddr_in *premote_addr, char *pusrdata,
		unsigned short length) {

	ESP_LOGI(TAG, "DNS Request received.");
	char *buff = (char*) pvPortMalloc(LENGTH_DNS);
	char *reply = (char*) pvPortMalloc(LENGTH_DNS);
	char *rend = reply + length;
	char *p = pusrdata;
	DnsHeader *hdr = (DnsHeader*) p;
	DnsHeader *rhdr = (DnsHeader*) reply;
	p += sizeof(DnsHeader);

	//DNS Implementaiton ist zu lang
	if (length > 512)
		goto finish;
	//DNS Implementaiton ist zu kurz
	if (length < sizeof(DnsHeader))
		goto finish;
	//DNS Antworten werden nicht bearbeitet
	if (hdr->ancount || hdr->nscount || hdr->arcount)
		goto finish;
	if (hdr->flags & FLAG_TC)
		goto finish;

	memcpy(reply, pusrdata, length);
	rhdr->flags |= FLAG_QR;
	for (int i = 0; i < local_ntohs(&hdr->qdcount); i++) {

		p = labelToStr(pusrdata, p, length, buff, LENGTH_DNS);
		if (p == NULL)
			goto finish;
		DnsQuestionFooter *qf = (DnsQuestionFooter*) p;
		p += sizeof(DnsQuestionFooter);
		if (local_ntohs(&qf->type) == RECORD_TYPE_A) {
			//Erstellen der Antwort
			rend = strToLabel(buff, rend, LENGTH_DNS - (rend - reply)); //Add the label
			if (rend == NULL)
				goto finish;
			//Initieren des DNS Footer
			DnsResourceFooter *rf = (DnsResourceFooter *) rend;
			rend += sizeof(DnsResourceFooter);
			writeUInt16(&rf->type, RECORD_TYPE_A);
			writeUInt16(&rf->class, DNS_CLASS_IN);
			writeUInt32(&rf->ttl, 0);
			writeUInt16(&rf->rdlength, 4);
			//Klient benötigt die DNS Server Adresse -> ESP IP
			*rend++ = 192;
			*rend++ = 168;
			*rend++ = 4;
			*rend++ = 1;

			writeUInt16(&rhdr->ancount, local_ntohs(&rhdr->ancount) + 1);
		}
	}

	//Senden der Antwort
	sendto(sock_fd, reply, rend - reply, 0, (struct sockaddr * )premote_addr,
			sizeof(struct sockaddr_in));

	finish: if (buff) {
		free(buff);
		buff = NULL;
	}
	if (reply) {
		free(reply);
		reply = NULL;
	}
}

/*
 * Erstellen des Tasks mit dem die Socketverbindung aufgebaut wird
 */

void dns_task(void *pvParameters) {
	struct sockaddr_in server_addr;
	int ret;

	struct sockaddr_in from;
	socklen_t fromlen;

	int nNetTimeout = 10000;			// 10 Sec
	char *udp_msg = (char*) os_zalloc(LENGTH_DNS);

	//Setzen des DNS Ports und des Adressraums
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(DNS_SERVER_PORT);
	server_addr.sin_len = sizeof(server_addr);

	do {
		sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
		if (sock_fd == -1) {
			ESP_LOGI(TAG, "Socket konnte nicht gestartet werden");
			vTaskDelay(1000 / portTICK_RATE_MS);
		}
	} while (sock_fd == -1);

	do {
		ret = bind(sock_fd, (struct sockaddr * )&server_addr,
				sizeof(server_addr));
		if (ret != 0) {
			ESP_LOGI(TAG, "Socket konnte nicht gebunden werden.");
			vTaskDelay(1000 / portTICK_RATE_MS);
		}
	} while (ret != 0);

	while (1) {

		memset(udp_msg, 0, LENGTH_DNS);
		memset(&from, 0, sizeof(from));

		setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, (char * )&nNetTimeout,
				sizeof(int));
		fromlen = sizeof(struct sockaddr_in);

		ret = lwip_recvfrom(sock_fd, udp_msg, LENGTH_DNS, 0, &from, &fromlen);
		if (ret > 0) {
			dnsRecv(&from, udp_msg, ret);
		}

		ESP_LOGI(TAG, "DNS stack %d, HEAP %d\n",
				(int)uxTaskGetStackHighWaterMark(NULL),
				(int) heap_caps_get_free_size(MALLOC_CAP_8BIT));
	}

	if (udp_msg) {
		free(udp_msg);
		udp_msg = NULL;
	}
	close(sock_fd);
	vTaskDelete(NULL);

}

void startDNS(void) {

	ESP_LOGI(TAG, "DNS Task wird gestartet");
	if (!dns_task_initialized) {
		xTaskCreate(&dns_task, "dns_task", 4096, NULL, 3, &dns_handle);
		dns_task_initialized = 1;
	}
}

