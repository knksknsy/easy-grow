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

void initFlashWriter();
void flash_write(int const address, uint8_t const value);


#endif /* MAIN_INCLUDE_EASY_PERSISTENCE_H_ */
