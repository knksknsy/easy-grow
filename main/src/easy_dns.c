/*
 * easy_dns.c
 *
 *  Created on: 23 Jun 2019
 *      Author: m.bilge
 *
 *
 *  DNS Server using
 *
 *
 *
 */

#include "FreeRTOS.h"
#include "task.h"

#include "queue.h"

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

#define TAG "EASY_DNS"


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
	uint8_t len;
	uint8_t data;
} DnsLabel;


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

typedef struct __attribute__ ((packed)) {
	uint16_t prio;
	uint16_t weight;
} DnsUriHdr;


#define FLAG_QR (1<<7)
#define FLAG_AA (1<<2)
#define FLAG_TC (1<<1)
#define FLAG_RD (1<<0)

#define QTYPE_A  1
#define QTYPE_NS 2
#define QTYPE_CNAME 5
#define QTYPE_SOA 6
#define QTYPE_WKS 11
#define QTYPE_PTR 12
#define QTYPE_HINFO 13
#define QTYPE_MINFO 14
#define QTYPE_MX 15
#define QTYPE_TXT 16
#define QTYPE_URI 256

#define QCLASS_IN 1
#define QCLASS_ANY 255
#define QCLASS_URI 256

#define DNS_LEN 512
//Function to put unaligned 16-bit network values
static void   setn16(void *pp, int16_t n) {
	char *p=pp;
	*p++=(n>>8);
	*p++=(n&0xff);
}

//Function to put unaligned 32-bit network values
static void   setn32(void *pp, int32_t n) {
	char *p=pp;
	*p++=(n>>24)&0xff;
	*p++=(n>>16)&0xff;
	*p++=(n>>8)&0xff;
	*p++=(n&0xff);
}


static uint16_t   local_ntohs(uint16_t *in) {
	char *p=(char*)in;
	return ((p[0]<<8)&0xff00)|(p[1]&0xff);
}


//Parses a label into a C-string containing a dotted
//Returns pointer to start of next fields in packet
static char*   labelToStr(char *packet, char *labelPtr, int packetSz, char *res, int resMaxLen) {
	int i, j, k;
	char *endPtr=NULL;
	i=0;
	do {
		if ((*labelPtr&0xC0)==0) {
			j=*labelPtr++; //skip past length
			//Add separator period if there already is data in res
			if (i<resMaxLen && i!=0) res[i++]='.';
			//Copy label to res
			for (k=0; k<j; k++) {
				if ((labelPtr-packet)>packetSz) return NULL;
				if (i<resMaxLen) res[i++]=*labelPtr++;
			}
		} else if ((*labelPtr&0xC0)==0xC0) {
			//Compressed label pointer
			endPtr=labelPtr+2;
			int offset=local_ntohs(((uint16_t *)labelPtr))&0x3FFF;
			//Check if offset points to somewhere outside of the packet
			if (offset>packetSz) return NULL;
			labelPtr=&packet[offset];
		}
		//check for out-of-bound-ness
		if ((labelPtr-packet)>packetSz) return NULL;
	} while (*labelPtr!=0);
	res[i]=0; //zero-terminate
	if (endPtr==NULL) endPtr=labelPtr+1;
	return endPtr;
}


//Converts a dotted hostname to the weird label form dns uses.
static char   *strToLabel(char *str, char *label, int maxLen) {
	char *len=label; //ptr to len byte
	char *p=label+1; //ptr to next label byte to be written
	while (1) {
		if (*str=='.' || *str==0) {
			*len=((p-len)-1);	//write len of label bit
			len=p;				//pos of len for next part
			p++;				//data ptr is one past len
			if (*str==0) break;	//done
			str++;
		} else {
			*p++=*str++;	//copy byte
//			if ((p-label)>maxLen) return NULL;	//check out of bounds
		}
	}
	*len=0;
	return p; //ptr to first free byte in resp
}

#define DNS_SERVER_PORT     53

int sock_fd;
//LOCAL xQueueHandle QueueStop = NULL;

//Receive a DNS packet and maybe send a response back
static void   dnsRecv(struct sockaddr_in *premote_addr, char *pusrdata, unsigned short length) {

    ESP_LOGI(TAG,"DNS request received.");
 	char *buff = (char*) pvPortMalloc(DNS_LEN);
	char *reply = (char*)pvPortMalloc(DNS_LEN);
	char *rend=reply+length;
	char *p=pusrdata;
	DnsHeader *hdr=(DnsHeader*)p;
	DnsHeader *rhdr=(DnsHeader*)reply;
	p+=sizeof(DnsHeader);

	if (length>512) goto finish; 									//Packet is longer than DNS implementation allows,512
	if (length<sizeof(DnsHeader)) goto finish; 						//Packet is too short
	if (hdr->ancount || hdr->nscount || hdr->arcount) goto finish;	//this is a reply, don't know what to do with it
	if (hdr->flags&FLAG_TC) goto finish;								//truncated, can't use this

	memcpy(reply, pusrdata, length);
	rhdr->flags|=FLAG_QR;
	for (int i=0; i<local_ntohs(&hdr->qdcount); i++) {

		p=labelToStr(pusrdata, p, length, buff, DNS_LEN);
		if (p==NULL) goto finish;
		DnsQuestionFooter *qf=(DnsQuestionFooter*)p;
		p+=sizeof(DnsQuestionFooter);
		ESP_LOGI(TAG,"DNS: Q (type 0x%X class 0x%X) for %s\n", local_ntohs(&qf->type), local_ntohs(&qf->class), buff);
		if (local_ntohs(&qf->type)==QTYPE_A) {
			//They want to know the IPv4 address of something.
			//Build the response.
        	ESP_LOGI(TAG,"DNS Request received");
			rend=strToLabel(buff, rend, DNS_LEN-(rend-reply)); //Add the label
			ESP_LOGI(TAG,"DNS 2");
			if (rend==NULL) goto finish;
			ESP_LOGI(TAG,"DNS 3");
			DnsResourceFooter *rf=(DnsResourceFooter *)rend;
			rend+=sizeof(DnsResourceFooter);
			setn16(&rf->type, QTYPE_A);
			setn16(&rf->class, QCLASS_IN);
			setn32(&rf->ttl, 0);
			setn16(&rf->rdlength, 4); //IPv4 addr is 4 bytes;
			ESP_LOGI(TAG,"DNS 4");
			//Grab the current IP of the softap interface

			*rend++=192;
			*rend++=168;
			*rend++=4;
			*rend++=1;

			setn16(&rhdr->ancount, local_ntohs(&rhdr->ancount)+1);
		} else if (local_ntohs(&qf->type)==QTYPE_NS) {
			rend=strToLabel(buff, rend, DNS_LEN-(rend-reply)); //Add the label
			DnsResourceFooter *rf=(DnsResourceFooter *)rend;
			rend+=sizeof(DnsResourceFooter);
			setn16(&rf->type, QTYPE_NS);
			setn16(&rf->class, QCLASS_IN);
			setn16(&rf->ttl, 0);
			setn16(&rf->rdlength, 4);
			*rend++=2;
			*rend++='n';
			*rend++='s';
			*rend++=0;
			setn16(&rhdr->ancount, local_ntohs(&rhdr->ancount)+1);
		} else if (local_ntohs(&qf->type)==QTYPE_URI) {
			//Give uri to us
        	ESP_LOGI(TAG,"URI wanted");
			rend=strToLabel(buff, rend, DNS_LEN-(rend-reply)); //Add the label
			DnsResourceFooter *rf=(DnsResourceFooter *)rend;
			rend+=sizeof(DnsResourceFooter);
			DnsUriHdr *uh=(DnsUriHdr *)rend;
			rend+=sizeof(DnsUriHdr);
			setn16(&rf->type, QTYPE_URI);
			setn16(&rf->class, QCLASS_URI);
			setn16(&rf->ttl, 0);
			setn16(&rf->rdlength, 4+16);
			setn16(&uh->prio, 10);
			setn16(&uh->weight, 1);
			memcpy(rend, "192.168.4.1", 16);
			rend+=16;
			setn16(&rhdr->ancount, local_ntohs(&rhdr->ancount)+1);
//			printf("Added NS rec to resp. Resp len is %d\n", (rend-reply));
		}
	}

	//Send the response
    sendto(sock_fd,reply, rend-reply, 0, (struct sockaddr *)premote_addr, sizeof(struct sockaddr_in));

finish:
    if(buff){
        free(buff);
        buff = NULL;
    }
    if(reply){
        free(reply);
        reply = NULL;
    }
}

/******************************************************************************
 * FunctionName : user_devicefind_init
 * Description  : the espconn struct parame init
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void
dns_task(void *pvParameters)
{
    struct sockaddr_in server_addr;
    int ret;

    struct sockaddr_in from;
    socklen_t   fromlen;
    //struct ip_info ipconfig;

    int nNetTimeout=10000;// 10 Sec
    char *udp_msg = (char*)os_zalloc(DNS_LEN);

    //memset(&ipconfig, 0, sizeof(ipconfig));
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(DNS_SERVER_PORT);
    server_addr.sin_len = sizeof(server_addr);

    do{
        sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock_fd == -1) {
        	ESP_LOGI(TAG,"captdns_task failed to create sock!\n");
            vTaskDelay(1000/portTICK_RATE_MS);
        }
    }while(sock_fd == -1);

    do{
        ret = bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
        if (ret != 0) {
        	ESP_LOGI(TAG,"captdns_task failed to bind sock!\n");
            vTaskDelay(1000/portTICK_RATE_MS);
        }
    }while(ret != 0);

    while(1){

        memset(udp_msg, 0, DNS_LEN);
        memset(&from, 0, sizeof(from));

        setsockopt(sock_fd,SOL_SOCKET,SO_RCVTIMEO,(char *)&nNetTimeout,sizeof(int));
        fromlen = sizeof(struct sockaddr_in);
        ret = lwip_recvfrom(sock_fd,  udp_msg, DNS_LEN, 0,&from,&fromlen);
        if (ret > 0) {
            //printf("captdns recv %d Bytes from Port %d %s\n",ret,ntohs(from.sin_port),inet_ntoa(from.sin_addr));
            dnsRecv(&from,udp_msg,ret);
        }

        ESP_LOGI(TAG,"captdns stack %d, heap %d\n",(int)uxTaskGetStackHighWaterMark(NULL),(int) heap_caps_get_free_size(MALLOC_CAP_8BIT));
    }

    if(udp_msg){
        free(udp_msg);
        udp_msg = NULL;
    }
    close(sock_fd);
    vTaskDelete(NULL);

}

void startDNS(void)
{
    ESP_LOGI(TAG,"DNS Task started.");
    xTaskCreate(&dns_task, "dns_task", 1024, NULL, 3, NULL);//1024,866,192

}





