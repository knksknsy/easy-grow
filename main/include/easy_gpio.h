/*
 * easy_gpio.h
 *
 *  Created on: 7 May 2019
 *      Author: m.bilge
 */

#include "driver/gpio.h"

#ifndef MAIN_INCLUDE_EASY_GPIO_H_

#define MAIN_INCLUDE_EASY_GPIO_H_


#define GPIO_HIGH 1
#define GPIO_LOW 0

#define WATER_LOW 0
#define WATER_MEDIUM 1
#define WATER_HIGH 2

static void gpio_task_input_received(void *arg);

static void gpio_isr_handler(void *arg);

void gpio_task(int pin,int state);

void checkStates();

void blinkTask();

void init_gpio_output(gpio_config_t *io_config);

void init_gpio_input(gpio_config_t *io_config);

void init_gpio();

#endif /* MAIN_INCLUDE_EASY_GPIO_H_ */

