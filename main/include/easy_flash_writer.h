/*
 * easy_flash_writer.c
 *
 *  Created on: 23 May 2019
 *      Author: Tim Tenckhoff
 */

#ifndef MAIN_INCLUDE_EASY_FLASH_WRITER_H
#define MAIN_INCLUDE_EASY_FLASH_WRITER_H

#define _FS_END 0x405FB000
#define _FS_RANGE 0x40200000

// Enum for the two different types of Data to flash
typedef enum FlashDataType
{
	SUN_COUNTER_PREV_STATE,
	SUN_COUNTER_PREV_TIME,
	SUN_COUNTER_TIME_DAY,
	SUN_COUNTER_TIME_NIGHT
} FlashDataType;

void initFlashWriter();
void flash_write(void* value, FlashDataType dataType);
uint8_t flash_read(void* value, FlashDataType dataType);

#endif /* MAIN_INCLUDE_EASY_PERSISTENCE_H_ */
