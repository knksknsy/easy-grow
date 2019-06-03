/*
 * easy_gpio.h
 *
 *  Created on: 7 May 2019
 *  Author:		m.bilge, Kaan Keskinsoy
 */

#ifndef MAIN_INCLUDE_EASY_GPIO_H_
#define MAIN_INCLUDE_EASY_GPIO_H_

#define GPIO_HIGH		1
#define GPIO_LOW		0

#define WATER_LOW		0
#define WATER_MEDIUM	1
#define WATER_HIGH		2

typedef enum { UP, DOWN, PRESS, RELEASE } ButtonStates;

void moisture_button_handler(uint32_t io_num);
void photo_diode_handler(uint32_t io_num);
void water_level_handler(uint32_t io_num);

void init_isr();
void init_gpio();

void init_pump_output();
void init_moisture_buttons_input();
void init_moisture_leds_output();
void init_water_level_sensors_input();
void init_water_level_leds_output();
void init_photo_diode_input();
void init_moisture_sensor_adc_input();

void setMoistureLevel(int *level);

#endif /* MAIN_INCLUDE_EASY_GPIO_H_ */
