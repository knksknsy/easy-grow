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


#define FS_END 0x411FB000

#define TAG "Flash Writer"


uint32_t _startSector = (((uint32_t) FS_END - 0x40200000) / SPI_FLASH_SEC_SIZE);

uint8_t SPI_FLASH_RESULT_OK = 0;

size_t _size = 1;
uint8_t* _data;
_Bool _dirty = false;
_Bool ret = false;


uint8_t flash_read(uint8_t const address) {
	  ESP_LOGI(TAG, "reading data from flash.");

	  int result = -1;
	  result = spi_flash_read(_startSector * SPI_FLASH_SEC_SIZE, (uint32_t*)(_data), _size);

	  if (result != -1)
	  {
		ESP_LOGI(TAG, "Success: [%d]", result);
	  }

  return result;
}




void flash_write(int const address, uint8_t const value) {

    ESP_LOGI(TAG, "writing to sector [%d], size: %d\n", _startSector, SPI_FLASH_SEC_SIZE);

	uint8_t* pData = &_data[address];
    ESP_LOGI(TAG, "Data: [%d]", (int)pData);


	  if (*pData != value)
	  {
	    *pData = value;

	  }

	ESP_LOGI(TAG, "Result: [%d]", spi_flash_erase_sector(_startSector * SPI_FLASH_SEC_SIZE));

	if(spi_flash_erase_sector(_startSector * SPI_FLASH_SEC_SIZE) == SPI_FLASH_RESULT_OK) {
		ESP_LOGI(TAG, "writing sth to flash");

	    if(spi_flash_write(_startSector * SPI_FLASH_SEC_SIZE, (uint32_t*) (_data), _size) == SPI_FLASH_RESULT_OK){
	    	ret = true;

	    }
	    }
	 }



void initFlashWriter(){
	flash_write(_startSector,1);
    ESP_LOGI(TAG, "Reading Data: [%d]", flash_read(_startSector));

}


