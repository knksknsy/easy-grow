/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <esp_system.h>
#include <easy_http_server.h>
#include <easy_gpio.h>
#include <easy_wifi_manager.h>

#include <esp_log.h>
#define TAG "Main"


#include "freertos/FreeRTOS.h"
#include <task.h>
#include <stdint.h>
#include "ws2812_i2s.h"

/******************************************************************************
 * FunctionName : app_main
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void app_main(void)
{
    ESP_LOGI(TAG, "app_main started");

	//init_gpio();
	init_output_moisture_leds();
	ws2812_i2s_init(10,);
	ws2812_pixel_t pixels = {255,22,33};
	ws2812_i2s_update(&pixels);



	wifi_init();

	//Change website to start ESP with AP Screen
    website_interface website = EASY_GROW_MODE;
    start_http(&website);

}
