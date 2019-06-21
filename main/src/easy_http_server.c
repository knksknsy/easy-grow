/*
 * easy_http_server.c
 *
 *  Created on: 22 Apr 2019
 *      Author: m.bilge
 */

#include <FreeRTOS.h>

#include <esp_wifi.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_err.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <lwip/api.h>

#include <easy_wifi_manager.h>
#include <easy_gpio.h>

#include <easy_http_server.h>
#include <easy_data.h>

#include <esp_http_server.h>
#include "esp_wifi.h"

/* A simple example that demonstrates how to create GET and POST
 * handlers for the web server.
 * The examples use simple WiFi configuration that you can set via
 * 'make menuconfig'.
 * If you'd rather not, just change the below entries to strings
 * with the config you want -
 * ie. #define EXAMPLE_WIFI_SSID "mywifissid"
 */
//#define EXAMPLE_WIFI_SSID "MartinRouterKing"
//#define EXAMPLE_WIFI_PASS "VgEp#x+c2d7MnVzK,;_="
static const char *TAG = "HTTP_SERVER";
static uint8_t available_aps[5][33];

webMode mode = EASY_CONFIG;

void decode_url(char *dest, const char *src) {
	const char *p = src;
	char code[3] = { 0 };
	unsigned long ascii = 0;
	char *end = NULL;

	while (*p) {
		if (*p == '%') {
			memcpy(code, ++p, 2);
			ascii = strtoul(code, &end, 16);
			*dest++ = (char) ascii;
			p += 2;
		} else
			*dest++ = *p++;
	}
}


void httpd_task_config(void *pvParameters) {
	ESP_LOGI(TAG, "CONFIG TASK STARTED");

	struct netconn *client = NULL;
	struct netconn *nc = netconn_new(NETCONN_TCP);
	if (nc == NULL) {
		printf("Failed to allocate socket.\n");
		vTaskDelete(NULL);
	}
	netconn_bind(nc, IP_ADDR_ANY, 80);
	netconn_listen(nc);

	char buf[1024];

	while (1) {
		err_t err = netconn_accept(nc, &client);
		if (err == ERR_OK) {
			struct netbuf *nb;
			if ((err = netconn_recv(client, &nb)) == ERR_OK) {
				void *data;
				u16_t len;
				netbuf_data(nb, &data, &len);
				/* check for a GET request */
				if (!strncmp(data, "GET ", 4)) {
					char uri[115];
					const int max_uri_len = 115;
					char *sp1, *sp2;
					/* extract URI */
					sp1 = data + 4;
					sp2 = memchr(sp1, ' ', max_uri_len);
					int len = sp2 - sp1;
					memcpy(uri, sp1, len);
					uri[len] = '\0';
					printf("uri: %s\n", uri);
					char url_decoded[sizeof uri] = { 0 };
					decode_url(url_decoded, uri);
					char delimiter[] = "?=&";
					char *ptr;
					// initialisieren und ersten Abschnitt (URL) auslesen
					ptr = strtok(url_decoded, delimiter);
					if (!strncmp(ptr, "/submit", max_uri_len)) {
						// �bern�chsten Abschnitt (SSID) auslesen
						ptr = strtok(NULL, delimiter);
						ptr = strtok(NULL, delimiter);
						char ssid[32];
						memcpy(ssid, ptr, 32);
						// �bernaechsten Abschnitt (PW) auslesen
						ptr = strtok(NULL, delimiter);
						ptr = strtok(NULL, delimiter);
						char pwd[64];
						memcpy(pwd, ptr, 64);

						ESP_LOGI(TAG, "SUBMITTED");

						mode = EASY_MOISTURE;
						sta_wifi_init(ssid, pwd);

						//start_config_http();
						//vTaskSuspendAll();

						//vTaskDelete(NULL);
						//break;

					} else if (!strncmp(ptr, "/higher", max_uri_len)) {
						ESP_LOGI(TAG, "helloo");
						//TODO
						//setMoistureHigher(true);
					} else if (!strncmp(ptr, "/lower", max_uri_len)) {
						//TODO
						//setMoistureHigher(false);
					} else if (!strncmp(ptr, "/ap", max_uri_len)) {
						mode = EASY_MOISTURE;

					} else if (!strncmp(ptr, "/reset", max_uri_len)) {
						reset_wifi_credentials();

					} else {

					}
					char webpage[1024];
					strcpy(webpage, WEBPAGE_HEAD);
					if (mode == EASY_CONFIG) {
						strcat(webpage , WEBPAGE_CONFIG);
						snprintf(buf, sizeof(buf),
								webpage, available_aps[0],
								available_aps[1], available_aps[2],
								available_aps[3], available_aps[4]);
					} else if (mode == EASY_REDIRECT) {
						strcat(webpage , WEBPAGE_CONFIG);
						snprintf(buf, sizeof(buf),
								webpage);
					} else if (mode == EASY_MOISTURE) {
						strcat(webpage , WEBPAGE_MOISTURE);
						snprintf(buf, sizeof(buf),
								webpage, uri,
								xTaskGetTickCount() * portTICK_PERIOD_MS / 1000,
								(int) heap_caps_get_free_size(MALLOC_CAP_8BIT));
					}

					netconn_write(client, buf, strlen(buf), NETCONN_COPY);
				}
			}
			netbuf_delete(nb);
		}
		printf("Closing connection\n");
		netconn_close(client);
		netconn_delete(client);
	}
}

void start_config_http(webMode webMode) {
	mode = webMode;
	ESP_LOGI(TAG, "SERVER STARTED");
	xTaskCreate(&httpd_task_config, "wifi_config_server", 4096, NULL, 2, NULL);
}


/*
 * set access points to display in webpage
 *
 * wifi_ap_record_t aps[] all found access points
 * uint16_t apCount amount of access points found
 */
void set_aps(wifi_ap_record_t aps[], uint16_t apCount) {
	for (int i = 0; i < 5; i++) {
		ESP_LOGI(TAG, &aps[i].ssid);
		// if no aps found in scan, fill first entry with error message
		if (i == 0 && apCount == 0) {
			memcpy(available_aps[i], "Kein Wlan gefunden!",
					sizeof(available_aps[i]));
		}
		// if less than 5 aps found in scan, fill with empty strings
		else if (i >= apCount) {
			memcpy(available_aps[i], "", sizeof(available_aps[i]));
		} else {
			memcpy(available_aps[i], &aps[i].ssid, sizeof(available_aps[i]));
		}
	}
}

