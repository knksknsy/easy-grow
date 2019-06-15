/*
 * easy_controller.h
 *
 *  Created on: 3 Jun 2019
 *      Author: Kaan Keskinsoy
 */

#ifndef MAIN_INCLUDE_EASY_CONTROLLER_H_
#define MAIN_INCLUDE_EASY_CONTROLLER_H_

typedef enum { EMPTY, LOW, FULL } WaterLevel;

void moisture_button_handler(uint32_t io_num);
void moisture_leds_handler(uint8_t level);
void set_moisture_level(uint8_t level);
uint8_t get_moisture_level();

void photo_diode_handler(uint32_t io_num);
uint8_t get_hours_of_sun();

static void water_low_task(void *arg);
void water_level_handler(uint32_t io_num);
WaterLevel get_water_level();

void activate_pump(uint32_t ms);
void deactivate_pump();

//void setMoistureLevel(int *level);

#endif /* MAIN_INCLUDE_EASY_CONTROLLER_H_ */
