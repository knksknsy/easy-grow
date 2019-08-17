/*
 * easy_persistence.c
 *
 *  Created on: 23 May 2019
 *      Author: Tim Tenckhoff
 */
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include <stdint.h>
#include <stdio.h>
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "tcpip_adapter.h"
#include "esp_smartconfig.h"

#include "spi_flash.h"

#include "easy_flash_writer.h"

#include <stdbool.h>

#define _FS_END 0x405FB000

#define TAG "Flash Writer"

uint32_t _startSector = (((uint32_t) _FS_END - 0x40200000) / SPI_FLASH_SEC_SIZE);


uint8_t SPI_FLASH_RESULT_OK = 0;


uint8_t addr = 0;

_Bool ret = false;

uint32_t getStartSector(FlashDataType dataType){
	uint32_t startSector = 0;
			switch(dataType) {
			   case FLASHDAYS  :
				   return _startSector;
			      break;
			   case FLASHHOURS  :
				   return _startSector/2;
			   	  break;
			   default :
				   return startSector;
				   break;
			}

}

uint8_t flash_read(void* value, FlashDataType dataType) {
	uint32_t startSector = getStartSector(dataType);

	int result = -1;
	result = spi_flash_read(startSector * SPI_FLASH_SEC_SIZE,
			value, sizeof(value));

	if (result != -1) {
		ESP_LOGI(TAG, "[flash_read]:Successfully read data.");
	}else {
		ESP_LOGI(TAG, "[flash_read]Error occured while reading flash, Error Code: [%d]", result);

	}

	return result;
}



void flash_write(void* value, FlashDataType dataType) {
	uint32_t startSector = getStartSector(dataType);

	ESP_LOGI(TAG, "[flash_write]: Writing value to sector [%d], size: %d\n",startSector, SPI_FLASH_SEC_SIZE);

	esp_err_t status = spi_flash_erase_sector(startSector);
	if (status == SPI_FLASH_RESULT_OK) {

		status = spi_flash_write(startSector * SPI_FLASH_SEC_SIZE,
				 value, sizeof(value));

		if (status == SPI_FLASH_RESULT_OK) {
			ESP_LOGI(TAG, "[flash_write]: Successfully written to flash!");
			ret = true;
		}
		else {
			ESP_LOGI(TAG, "[flash_write]: Error with writing to flash, Error Code: [%d]", status);
		}
}}

