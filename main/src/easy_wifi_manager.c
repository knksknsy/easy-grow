/*
 * easy_wifimanager.c
 *
 *  Created on: 24 May 2019
 *      Author: m.bilge
 */
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include <event_groups.h>
#include "rom/ets_sys.h"


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

#define EXAMPLE_ESP_WIFI_SSID      "EasyGrow_Initial_Config"
#define EXAMPLE_ESP_WIFI_PASS	   "1Love_Ea5yGr0w"
#define EXAMPLE_MAX_STA_CONN	   5


const int WIFI_CONNECTED_BIT = BIT0;

#define TAG "Wifi_Manager"

static EventGroupHandle_t wifi_event_group;

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_SCAN_DONE: {
        uint16_t apCount = 0;
        esp_wifi_scan_get_ap_num(&apCount);
        if (apCount == 0) {
        	ESP_LOGI(TAG,"No AP found");
            //break;
        	//TODO Implemtend rescan function
        }
        wifi_ap_record_t *ap_list = (wifi_ap_record_t *)malloc(sizeof(wifi_ap_record_t) * apCount);
        if (!ap_list) {
        	ESP_LOGI(TAG, "malloc error, ap_list is NULL");
            break;
        }
        ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&apCount, ap_list));
        set_aps(ap_list, apCount);
        esp_wifi_scan_stop();
        free(ap_list);
    	//Start config http server
        start_config_http();
        break;
    }
    case SYSTEM_EVENT_STA_START:
    	ESP_LOGI(TAG, "STATION STARTED");
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        ESP_LOGI(TAG, "got ip:%s",
                 ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
        start_easy_grow_http();

        break;
    case SYSTEM_EVENT_AP_STACONNECTED:
        ESP_LOGI(TAG, "station:"MACSTR" join, AID=%d",
                 MAC2STR(event->event_info.sta_connected.mac),
                 event->event_info.sta_connected.aid);
        break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
        ESP_LOGI(TAG, "station:"MACSTR"leave, AID=%d",
                 MAC2STR(event->event_info.sta_disconnected.mac),
                 event->event_info.sta_disconnected.aid);
        break;
    case SYSTEM_EVENT_STA_CONNECTED:
    	ESP_LOGI(TAG,"Connected to STATION");
    	//Connected to accesspoint - Start EasyGrow Task
    	start_easy_grow_http();
    	break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
    	ESP_LOGI(TAG,"Disconnected from STATION");
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}

void wifi_init(char ssid[32], char pwd[64])
{
	esp_err_t err = esp_wifi_stop();
	if(err == ERR_OK)
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
	    if(err == ERR_OK)
	    {
	    	esp_err_t err = esp_wifi_disconnect();
	    	if(err == ERR_OK) {
	    		ESP_ERROR_CHECK(esp_wifi_connect());
	    	}
		    ESP_LOGI(TAG, "wifi_init_sta finished.");
		    ESP_LOGI(TAG, "connect to ap SSID:%s password:%s",
		             (char* )&ssid_int, (char* )&pwd_int);
	    }
	}
}

void initialise_ap()
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

    // TODO: Look for saved credentials and in case, immediately connect with wifi
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

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
	ESP_ERROR_CHECK(esp_wifi_start());

	ESP_LOGI(TAG, "wifi_init_softap finished.SSID:%s password:%s",
			 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);

	wifi_scan_config_t scanConf = {
		.ssid = NULL,
		.bssid = NULL,
		.channel = 0,
		.show_hidden = false
	};
	ESP_ERROR_CHECK(esp_wifi_scan_start(&scanConf, true));
}

