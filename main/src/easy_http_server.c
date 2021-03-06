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
#include <easy_controller.h>
#include <easy_gpio.h>

#include <easy_http_server.h>
#include <easy_data.h>
#include "easy_documentation.h"
#include <easy_flash_writer.h>

#include <easy_dns.h>

#include <esp_http_server.h>
#include "esp_wifi.h"

static const char *TAG = "HTTP_SERVER";
static uint8_t available_aps[5][33];

webMode mode = EASY_CONFIG;
int httpd_task_initialized = 0;

TaskHandle_t httpd_task_handle;
/*
 * Decode used for URL
 */


void decode(char *dest, const char *src) {
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
		} else if (*p == '+') {
			// space gets encoded as +, has to be decoded
	        *dest++ = ' ';
	        p++;
		} else
			*dest++ = *p++;
	}
}
const char* getMoistureOutputString(uint16_t level) {

	 switch (level)
	   {
	      case 0: return "Ausgeschaltet";
	      case 1: return "Niedrig";
	      case 2: return "Mittel";
	      case 3: return "Hoch";
	      default: return "Kein Sensor verbunden.";

	   }
	}

const char* getWaterLevelOutputString(WaterLevel level) {

	 switch (level)
	   {
	      case FULL: return "Voll";
	      case GOOD: return "Mittelvoll";
	      case EMPTY: return "Leer";
	      default: return "Kein Wassertank angeschlossen.";

	   }
	}

void httpd_task(void *pvParameters) {
	ESP_LOGI(TAG, "HTTPD TASK STARTED");
	struct netconn *client = NULL;
	struct netconn *nc = netconn_new(NETCONN_TCP);
	if (nc == NULL) {
		printf("Failed to allocate socket.\n");
		vTaskDelete(NULL);
	}
	netconn_bind(nc, IP_ADDR_ANY, 80);
	netconn_listen(nc);
	char buf[5500];

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
					// slice url at delimiters to extract ssid and pwd
					char delimiter[] = "?=&";
					char *ptr;
					// initialise and extract first part (URL)
					ptr = strtok(uri, delimiter);
					if (!strncmp(ptr, "/submit", max_uri_len)) {
						// extract value of next part (SSID)
						ptr = strtok(NULL, delimiter);
						ptr = strtok(NULL, delimiter);
						// Only continue if user actually entered a SSID (otherwise it immediately gets to 'PW' query parameter)
						if(strcmp(ptr, "PW")!=0) {
							char ssid[32];
							char ptr_ssid_decoded[sizeof ssid] = { 0 };
							decode(ptr_ssid_decoded, ptr);
							memcpy(ssid, ptr_ssid_decoded, 32);
							// extract value of next part (PW)
							ptr = strtok(NULL, delimiter);
							char pwd[64];
							pwd[0] = '\0';
							char ptr_pwd_decoded[sizeof pwd] = { 0 };
							// Only read pwd if user actually entered something
							while(ptr != NULL) {
								if(strcmp(ptr, "PW")!=0) {
									decode(ptr_pwd_decoded, ptr);
									memcpy(pwd, ptr_pwd_decoded, 64);
								}
								ptr = strtok(NULL, delimiter);
							}
							sta_wifi_init(ssid, pwd);
							vTaskSuspend(dns_handle);
							vTaskSuspend(httpd_task_handle);
						}
					} else if (!strncmp(ptr, "/high", max_uri_len)) {
						set_moisture_level(HIGH);
					} else if (!strncmp(ptr, "/medium", max_uri_len)) {
						set_moisture_level(MID);
					} else if (!strncmp(ptr, "/low", max_uri_len)) {
						set_moisture_level(LOW);
					} else if (!strncmp(ptr, "/off", max_uri_len)) {
						set_moisture_level(OFF);
					} else if (!strncmp(ptr, "/ap", max_uri_len)) {
						mode = EASY_MOISTURE;
					} else if (!strncmp(ptr, "/docu", max_uri_len)) {
						mode = EASY_DOCUMENTATION;
					} else if (!strncmp(ptr, "/pump", max_uri_len)) {
						activate_pump(0);
					} else if (!strncmp(ptr, "/reset", max_uri_len)) {
						reset_wifi_credentials();

					} else {

					}
					char webpage[5500];
					strcpy(webpage, WEBPAGE_HEAD);
					if (mode == EASY_CONFIG) {
						strcat(webpage, WEBPAGE_NEW_CONFIG);
						snprintf(buf, sizeof(buf), webpage, available_aps[0],
								available_aps[1], available_aps[2],
								available_aps[3], available_aps[4]);
					} else if (mode == EASY_DOCUMENTATION) {
						strcat(webpage, WEBPAGE_DOCUMENTATION);
						snprintf(buf, sizeof(buf), webpage);
					} else if (mode == EASY_MOISTURE) {
						MoistureValue mv = get_moisture_level();
						strcat(webpage, WEBPAGE_MOISTURE);

						char *moistureVal = getMoistureOutputString(mv.level_target);

						char *waterLevel = getMoistureOutputString(get_water_level());

						snprintf(buf, sizeof(buf), webpage, mv.level_target,
								moistureVal, mv.level_percentage, waterLevel,
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

void start_http(webMode webMode) {
	mode = webMode;

	if(httpd_task_initialized) {
		vTaskResume(httpd_task_handle);
	} else {
		xTaskCreate(&httpd_task, "wifi_config_server", 14096, NULL, 2, &httpd_task_handle);
		httpd_task_initialized = 1;
	}
}

/*
 * set access points to display in webpage
 *
 * wifi_ap_record_t aps[] all found access points
 * uint16_t apCount amount of access points found
 */
void set_aps(wifi_ap_record_t aps[], uint16_t apCount) {
	for (int i = 0; i < 5; i++) {
		ESP_LOGI(TAG, "SSID: %s", (char *)&aps[i].ssid);
		// if no aps found in scan, fill first entry with error message
		if (i == 0 && apCount == 0) {
			memcpy(available_aps[i], "Kein Wlan gefunden. Reset?",
					sizeof(available_aps[i]));
		}
		// if less than 5 aps found in scan, fill rest with empty strings
		else if (i >= apCount) {
			memcpy(available_aps[i], "", sizeof(available_aps[i]));
		} else {
			memcpy(available_aps[i], &aps[i].ssid, sizeof(available_aps[i]));
		}
	}
}


