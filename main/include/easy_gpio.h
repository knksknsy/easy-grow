/*
 * easy_gpio.h
 *
 *  Created on: 7 May 2019
 *  Author:		m.bilge, Kaan Keskinsoy
 */

#ifndef MAIN_INCLUDE_EASY_GPIO_H_
#define MAIN_INCLUDE_EASY_GPIO_H_

/**
 *
 *  GPIO class to read sensors and do stuff
 *
 */
#define PUMP_D0_OUTPUT						16
#define LBUTTON_D1_INPUT					0
#define LED_MOISTURE_1_D2_OUTPUT			4
#define LED_MOISTURE_2_D3_OUTPUT			5
#define LED_MOISTURE_3_D4_OUTPUT			2
#define RBUTTON_D5_INPUT					14
#define WATER_LEVEL_TOP_D6_INPUT			12
#define WATER_LEVEL_BOTTOM_D7_INPUT			13
#define LED_WATER_LEVEL_TOP_D8_OUTPUT		15
#define PHOTO_DIODE_RX_INPUT				3
#define LED_WATER_LEVEL_BOTTOM_TX_OUTPUT	1

// 2 minutes interval: 60 * 1000000 * 2 = 120000000
#define MOISTURE_READ_INTERVAL              30000000
#define MOISTURE_READ_DEPTH                 100

#define MOISTURE_MAX_HIGH                   253
#define MOISTURE_MAX_LOW                    1023

#define MOISTURE_HIGH                       543
#define MOISTURE_MID                        733
#define MOISTURE_LOW                        923

// 2.5 seconds interval: 1000 * 2.5
#define PUMP_INTERVAL                       2500

static void gpio_isr_handler(void *arg);
static void gpio_task(void *arg);

void init_isr();
void init_gpio();

void init_pump_output();
void init_moisture_buttons_input();
void init_moisture_leds_output();
void init_water_level_sensors_input();
void init_water_level_leds_output();
void init_photo_diode_input();
void init_moisture_sensor_adc_input();
void init_adc_timer();

#endif /* MAIN_INCLUDE_EASY_GPIO_H_ */
