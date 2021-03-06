/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <esp_log.h>
#include <stdio.h>
#include <esp_system.h>
#include <easy_http_server.h>
#include <easy_gpio.h>
#include <easy_wifi_manager.h>
#include <easy_flash_writer.h>


#define TAG "Main"

/*
  Uncomment to enable logging
 */
#define DEBUG


/******************************************************************************
 * FunctionName : app_main
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void app_main(void)
{
   ESP_LOGI(TAG, "app_main started");
#ifndef DEBUG
   init_gpio();
#endif
   general_wifi_init();
}







