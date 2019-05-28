/*
 * easy_gpio.h
 *
 *  Created on: 7 May 2019
 *  Author:		m.bilge, Kaan Keskinsoy
 */

#include "driver/gpio.h"


#ifndef MAIN_INCLUDE_EASY_GPIO_H_

#define MAIN_INCLUDE_EASY_GPIO_H_


#define GPIO_HIGH 1
#define GPIO_LOW 0

#define WATER_LOW 0
#define WATER_MEDIUM 1
#define WATER_HIGH 2

typedef enum { UP, DOWN, PRESS, RELEASE } buttonStates;

// Example
void init_gpio_output_example(gpio_config_t *io_config);
void blinkTask();

buttonStates delay_debounce(buttonStates button_state, int button_value);

// Implementation
static void gpio_isr_handler_input_moisture_button_l(void *arg);
static void gpio_task_input_moisture_button_l(void *arg);
static void gpio_isr_handler_input_moisture_button_r(void *arg);
static void gpio_task_input_moisture_button_r(void *arg);
static void gpio_isr_handler_input_water_level_sensors(void *arg);
static void gpio_task_input_water_level_sensors(void *arg);
static void gpio_isr_handler_input_photo_diode(void *arg);
static void gpio_task_input_photo_diode(void *arg);

void init_output_pump();
void init_input_moisture_buttons();
void init_output_moisture_leds();
void init_input_water_level_sensors();
void init_output_water_level_leds();
void init_input_photo_diode();
void init_input_moisture_sensor();

void setMoistureLevel(int *level);

void init_gpio();

#endif /* MAIN_INCLUDE_EASY_GPIO_H_ */

