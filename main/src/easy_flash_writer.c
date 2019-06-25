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

uint8_t flash_read(void* value, size_t size) {
	ESP_LOGI(TAG, "reading data from flash.");

	int result = -1;
	result = spi_flash_read(_startSector * SPI_FLASH_SEC_SIZE,
			value, size);

	if (result != -1) {
		ESP_LOGI(TAG, "Success: [%d]", result);
	}

	return result;
}

void flash_write(int const address, void* value, size_t size) {

	ESP_LOGI(TAG, "writing to sector [%d], size: %d\n", _startSector,
			SPI_FLASH_SEC_SIZE);

	ESP_LOGI(TAG, "fuu [%d]", _startSector);


	esp_err_t status = spi_flash_erase_sector(_startSector);
	if (status == SPI_FLASH_RESULT_OK) {

		status = spi_flash_write(_startSector * SPI_FLASH_SEC_SIZE,
				 value, size);


		ESP_LOGI(TAG, "writing sth to flash");
		ESP_LOGI(TAG, "status: [%d]",status);


		if (status == SPI_FLASH_RESULT_OK) {
			ESP_LOGI(TAG, "status: [%d]",
							status);

			ret = true;
		}
	} else {
		ESP_LOGI(TAG, "Code: [%d]",
				status);

	}
}

void initFlashWriter() {

	//ESP_LOGI(TAG, "Reading Data: [%d]", flash_read(_startSector));

	uint32_t val = 123;
	flash_write(_startSector, &val , sizeof(val));

	uint32_t res = 0;

	flash_read(&res, sizeof(res));
	ESP_LOGI(TAG, "status: [%d]",res);

}

