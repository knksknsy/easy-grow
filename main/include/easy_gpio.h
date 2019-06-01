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

void init_gpio();

void init_output_pump();
void init_input_moisture_buttons();
void init_output_moisture_leds();
void init_input_water_level_sensors();
void init_output_water_level_leds();
void init_input_photo_diode();
void init_input_moisture_sensor();

void setMoistureLevel(int *level);

#endif /* MAIN_INCLUDE_EASY_GPIO_H_ */
