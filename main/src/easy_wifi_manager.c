/*
 * easy_wifimanager.c
 *
 *  Created on: 24 May 2019
 *      Author: m.bilge
 */
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include "freertos/task.h"
#include <event_groups.h>
#include "rom/ets_sys.h"
#include <time.h>


#include <esp_wifi.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_err.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <lwip/api.h>

#include <easy_wifi_manager.h>
#include <easy_http_server.h>
#include <easy_dns.h>


#define EXAMPLE_ESP_WIFI_SSID      "EasyGrow_Initial_Config"
#define EXAMPLE_ESP_WIFI_PASS	   ""
#define EXAMPLE_MAX_STA_CONN	   5


const int WIFI_CONNECTED_BIT = BIT0;
bool GOT_IP = false;
bool AP_ALREADY_INITIALIZED = false;
TaskHandle_t check_conn_handle;

#define TAG "Wifi_Manager"

static EventGroupHandle_t wifi_event_group;

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_SCAN_DONE: {
        uint16_t ap_count = 0;
        //get number of available access points
        esp_wifi_scan_get_ap_num(&ap_count);
        if (ap_count == 0) {
        	ESP_LOGI(TAG,"No AP found");
        }
        wifi_ap_record_t *ap_list = (wifi_ap_record_t *)malloc(sizeof(wifi_ap_record_t) * ap_count);
        if (!ap_list) {
        	ESP_LOGI(TAG, "malloc error, ap_list is NULL");
            break;
        }
        ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&ap_count, ap_list));
        set_aps(ap_list, ap_count);
        esp_wifi_scan_stop();
        free(ap_list);
    	//Start config http server
        start_http(EASY_CONFIG);
        break;
    }
    case SYSTEM_EVENT_STA_START:
    	ESP_LOGI(TAG, "STATION STARTED");
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        ESP_LOGI(TAG, "got ip:%s",
                 ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
        //Start moisture control http server
        start_http(EASY_MOISTURE);
        GOT_IP = true;
        break;
    case SYSTEM_EVENT_AP_STACONNECTED:
        ESP_LOGI(TAG, "station:"MACSTR" join, AID=%d",
                 MAC2STR(event->event_info.sta_connected.mac),
                 event->event_info.sta_connected.aid);
        startDNS();
        break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
        ESP_LOGI(TAG, "station:"MACSTR"leave, AID=%d",
                 MAC2STR(event->event_info.sta_disconnected.mac),
                 event->event_info.sta_disconnected.aid);
        break;
    case SYSTEM_EVENT_STA_CONNECTED:
    	ESP_LOGI(TAG,"Connected to STATION");
    	break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
    	ESP_LOGI(TAG,"Disconnected from STATION");
    	AP_ALREADY_INITIALIZED = true;
    	ap_wifi_init();
        //xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}

/*
 * Start wifi in STA-Mode (Connect to station)
 */
void sta_wifi_init(char ssid[32], char pwd[64])
{
	esp_err_t err = esp_wifi_stop();
	if(err == ESP_OK)
	{
		uint8_t ssid_int[32];
		memcpy(ssid_int, ssid, sizeof(uint8_t) * 32);
		uint8_t pwd_int[64];
		memcpy(pwd_int, pwd, sizeof(uint8_t) * 64);

	    wifi_config_t wifi_config_sta;
	    memset(&wifi_config_sta, 0, sizeof(wifi_config_sta));
	    memcpy(&wifi_config_sta.sta.ssid, &ssid_int, sizeof ssid_int);
	    memcpy(&wifi_config_sta.sta.password, &pwd_int, sizeof pwd_int);

	    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config_sta) );
	    esp_err_t err = esp_wifi_start();
	    if(err == ESP_OK)
	    {
	    	esp_err_t err = esp_wifi_disconnect();
	    	if(err == ESP_OK) {
	    		ESP_ERROR_CHECK(esp_wifi_connect());
	    	}
		    ESP_LOGI(TAG, "sta_wifi_init finished.");
		    ESP_LOGI(TAG, "connect to ap SSID:%s password:%s",
		             (char* )&ssid_int, (char* )&pwd_int);
	    }
	}
}

void reset_wifi_credentials()
{
	ESP_ERROR_CHECK(esp_wifi_disconnect());
}

/*
 * Task that waits for 10 seconds to check if esp actually connected to a station
 * inits ap mode if not
 */
void check_conn_task(void *pvParameters) {
	int time =  xTaskGetTickCount() * portTICK_PERIOD_MS / 1000;
	while(time < 10){
		time =  xTaskGetTickCount() * portTICK_PERIOD_MS / 1000;
	}
	if(GOT_IP == false) {
		ap_wifi_init();
	}
	vTaskDelete(check_conn_handle);
}

/*
 * Entry point for Wifi, connects to station if there are saved credentials,
 * otherwise opens up an access point
 */
void general_wifi_init()
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

	wifi_event_group = xEventGroupCreate();

    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	// Try connecting first, will connect to station if it has saved credentials
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_start());
	esp_err_t err = esp_wifi_connect();
	// if it doesn't return ESP_OK, start ap mode
	if (err != ESP_OK) {
    	ESP_LOGI(TAG,"Start in AP Mode");
		ap_wifi_init();
	} else {
		// BUG: After erase_flash, esp_wifi_connect returns ESP_OK but never actually connects
		// create task to wait and check if it really connected -> https://github.com/esp8266/Arduino/issues/2235
		xTaskCreate(&check_conn_task, "wifi_config_server", 4096, NULL, 2, &check_conn_handle);
	}
}

/*
 * Start wifi in AP-Mode (Access Point)
 */
void ap_wifi_init()
{
	wifi_config_t wifi_config = {
		.ap = {
			.ssid = EXAMPLE_ESP_WIFI_SSID,
			.ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
			.password = EXAMPLE_ESP_WIFI_PASS,
			.max_connection = EXAMPLE_MAX_STA_CONN,
			.authmode = WIFI_AUTH_WPA_WPA2_PSK
		},
	};
	if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0) {
		wifi_config.ap.authmode = WIFI_AUTH_OPEN;
	}
	//Has to be AP + Station mode, in order to scan for available aps
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
	ESP_ERROR_CHECK(esp_wifi_start());


	ESP_LOGI(TAG, "wifi_init_softap finished.SSID:%s password:%s",
			 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);

	wifi_scan_config_t scan_conf = {
		.ssid = NULL,
		.bssid = NULL,
		.channel = 0,
		.show_hidden = false
	};
	ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_conf, true));
}

