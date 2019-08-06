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

//Parses a label into a C-string containing a dotted
//Returns pointer to start of next fields in packet
static char* labelToStr(char *packet, char *labelPtr, int packetSz, char *res,
		int resMaxLen) {
	int i, j, k;
	char *endPtr = NULL;
	i = 0;
	do {
		if ((*labelPtr & 0xC0) == 0) {
			j = *labelPtr++; //skip past length
			//Add separator period if there already is data in res
			if (i < resMaxLen && i != 0)
				res[i++] = '.';
			//Copy label to res
			for (k = 0; k < j; k++) {
				if ((labelPtr - packet) > packetSz)
					return NULL;
				if (i < resMaxLen)
					res[i++] = *labelPtr++;
			}
		} else if ((*labelPtr & 0xC0) == 0xC0) {
			//Compressed label pointer
			endPtr = labelPtr + 2;
			int offset = local_ntohs(((uint16_t *) labelPtr)) & 0x3FFF;
			//Check if offset points to somewhere outside of the packet
			if (offset > packetSz)
				return NULL;
			labelPtr = &packet[offset];
		}
		//check for out-of-bound-ness
		if ((labelPtr - packet) > packetSz)
			return NULL;
	} while (*labelPtr != 0);
	res[i] = 0; //zero-terminate
	if (endPtr == NULL)
		endPtr = labelPtr + 1;
	return endPtr;
}

//Converts a dotted hostname to the weird label form dns uses.
static char *strToLabel(char *str, char *label, int maxLen) {
	char *len = label; //ptr to len byte
	char *p = label + 1; //ptr to next label byte to be written
	while (1) {
		if (*str == '.' || *str == 0) {
			*len = ((p - len) - 1);	//write len of label bit
			len = p;				//pos of len for next part
			p++;				//data ptr is one past len
			if (*str == 0)
				break;	//done
			str++;
		} else {
			*p++ = *str++;	//copy byte
//			if ((p-label)>maxLen) return NULL;	//check out of bounds
		}
	}
	*len = 0;
	return p; //ptr to first free byte in resp
}

#define DNS_SERVER_PORT     53

int sock_fd;
//LOCAL xQueueHandle QueueStop = NULL;

//Receive a DNS packet and maybe send a response back
static void dnsRecv(struct sockaddr_in *premote_addr, char *pusrdata,
		unsigned short length) {

	ESP_LOGI(TAG, "DNS request received.");
	char *buff = (char*) pvPortMalloc(LENGTH_DNS);
	char *reply = (char*) pvPortMalloc(LENGTH_DNS);
	char *rend = reply + length;
	char *p = pusrdata;
	DnsHeader *hdr = (DnsHeader*) p;
	DnsHeader *rhdr = (DnsHeader*) reply;
	p += sizeof(DnsHeader);

	if (length > 512)
		goto finish;
	//Packet is longer than DNS implementation allows,512
	if (length < sizeof(DnsHeader))
		goto finish;
	//Packet is too short
	if (hdr->ancount || hdr->nscount || hdr->arcount)
		goto finish;
	//this is a reply, don't know what to do with it
	if (hdr->flags & FLAG_TC)
		goto finish;
	//truncated, can't use this

	memcpy(reply, pusrdata, length);
	rhdr->flags |= FLAG_QR;
	for (int i = 0; i < local_ntohs(&hdr->qdcount); i++) {

		p = labelToStr(pusrdata, p, length, buff, LENGTH_DNS);
		if (p == NULL)
			goto finish;
		DnsQuestionFooter *qf = (DnsQuestionFooter*) p;
		p += sizeof(DnsQuestionFooter);
		if (local_ntohs(&qf->type) == RECORD_TYPE_A) {
			//They want to know the IPv4 address of something.
			//Build the response.
			rend = strToLabel(buff, rend, LENGTH_DNS - (rend - reply)); //Add the label
			if (rend == NULL)
				goto finish;
			DnsResourceFooter *rf = (DnsResourceFooter *) rend;
			rend += sizeof(DnsResourceFooter);
			writeUInt16(&rf->type, RECORD_TYPE_A);
			writeUInt16(&rf->class, DNS_CLASS_IN);
			writeUInt32(&rf->ttl, 0);
			writeUInt16(&rf->rdlength, 4); //IPv4 addr is 4 bytes;
			//Grab the current IP of the softap interface

			*rend++ = 192;
			*rend++ = 168;
			*rend++ = 4;
			*rend++ = 1;

			writeUInt16(&rhdr->ancount, local_ntohs(&rhdr->ancount) + 1);
		}
	}

	//Send the response
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

		ESP_LOGI(TAG, "captdns stack %d, heap %d\n",
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

