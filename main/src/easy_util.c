/*
 * easy_util.c
 *
 *  Created on:	3 August 2019
 *	Author:		m.bilge
 *
 *	Utility Class
 */

#include <easy_util.h>
#include <esp_system.h>



/*
 * Writes an 16 Bit Integer
 */
void writeUInt16(void *data, int16_t value) {
	char *d=data;
	*d++=(value>>8);
	*d++=(value&0xff);
}

/*
 * Writes an 32 Bit Integer
 */
void writeUInt32(void *data, int32_t value) {
	char *d=data;
	*d++=(value>>24)&0xff;
	*d++=(value>>16)&0xff;
	*d++=(value>>8)&0xff;
	*d++=(value&0xff);
}
