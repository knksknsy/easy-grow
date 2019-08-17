/*
 * persistence.h
 *
 *  Created on: 23 May 2019
 *      Author: Tim Tenckhoff
 */

#ifndef MAIN_INCLUDE_EASY_FLASH_WRITER_H
#define MAIN_INCLUDE_EASY_FLASH_WRITER_H

#define CONFIG_SECTOR 0x80-4
#define CONFIG_ADDRESS = (CONFIG_SECTOR * 4096)

// Enum for the two different types of Data to flash
typedef enum FlashDataType
{
    FLASHDAYS,
    FLASHHOURS,
} FlashDataType;

void initFlashWriter();
void flash_write(void* value, FlashDataType dataType);
uint8_t flash_read(void* value, FlashDataType dataType);





#endif /* MAIN_INCLUDE_EASY_PERSISTENCE_H_ */
