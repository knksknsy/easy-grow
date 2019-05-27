/*
 * easy_gpio.c
 *
 *  Created on:	7 May 2019
 *	Author:		m.bilge, Kaan Keskinsoy
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"

#include "esp_log.h"
#include "esp_system.h"
#include "easy_gpio.h"

#include "ws2812.h"


static const char *TAG = "GPIO";

/**
 *
 *  GPIO class to read sensors and do stuff
 *
 */
#define PUMP_D0_OUTPUT						16
#define LBUTTON_D1_INPUT					5
#define LED_MOISTURE_1_D2_OUTPUT			4
#define LED_MOISTURE_2_D3_OUTPUT			0
#define LED_MOISTURE_3_D4_OUTPUT			2
#define RBUTTON_D5_INPUT					14
#define WATER_LEVEL_TOP_D6_INPUT			12
#define WATER_LEVEL_BOTTOM_D7_INPUT			13
#define LED_WATER_LEVEL_TOP_D8_OUTPUT		15
#define PHOTO_DIODE_RX_INPUT				3
#define LED_WATER_LEVEL_BOTTOM_TX_OUTPUT	1
#define MOISTURE_SENSOR_A0_INPUT			0xA0

/**
 * GPIO definitions for example
 */
#define GPIO_OUTPUT_LED					2
#define GPIO_OUTPUT_IO_1    			16

static xQueueHandle gpio_event_queue_output_leds = NULL;
static xQueueHandle gpio_event_queue_input_moisture_buttons = NULL;
static xQueueHandle gpio_event_queue_input_water_level_sensors = NULL;
static xQueueHandle gpio_event_queue_input_photo_diode = NULL;


static void setMoistureLevel(int *level){
	uint32_t color = 0x00FF0000;
	ws2812_set_many(LED_MOISTURE_1_D2_OUTPUT, &color,(size_t) level);
}

static void gpio_isr_handler_output_leds(void *arg) {
	uint32_t gpio_num = (uint32_t) arg;
	xQueueSendFromISR(gpio_event_queue_output_leds, &gpio_num, NULL);
}

static void gpio_task_output_leds(void *arg) {
	uint32_t io_num;
	while (1) {
		if (xQueueReceive(gpio_event_queue_output_leds, &io_num, portMAX_DELAY)) {
			// TODO: Logic implementation
			ESP_LOGI(TAG, "GPIO[%d] intr, value: %d\n", io_num, gpio_get_level(io_num));
		}
	}
}

static void gpio_isr_handler_input_moisture_buttons(void *arg) {
	uint32_t gpio_num = (uint32_t) arg;
	xQueueSendFromISR(gpio_event_queue_input_moisture_buttons, &gpio_num, NULL);
}

static void gpio_task_input_moisture_buttons(void *arg) {
	uint32_t io_num;
	while (1) {
		if (xQueueReceive(gpio_event_queue_input_moisture_buttons, &io_num, portMAX_DELAY)) {
			// TODO: Logic implementation
			ESP_LOGI(TAG, "GPIO[%d] intr, value: %d\n", io_num, gpio_get_level(io_num));
		}
	}
}

static void gpio_isr_handler_input_water_level_sensors(void *arg) {
	uint32_t gpio_num = (uint32_t) arg;
	xQueueSendFromISR(gpio_event_queue_input_water_level_sensors, &gpio_num, NULL);
}

static void gpio_task_input_water_level_sensors(void *arg) {
	uint32_t io_num;
	while (1) {
		if (xQueueReceive(gpio_event_queue_input_water_level_sensors, &io_num, portMAX_DELAY)) {
			// TODO: Logic implementation
			ESP_LOGI(TAG, "GPIO[%d] intr, value: %d\n", io_num, gpio_get_level(io_num));
		}
	}
}

static void gpio_isr_handler_input_photo_diode(void *arg) {
	uint32_t gpio_num = (uint32_t) arg;
	xQueueSendFromISR(gpio_event_queue_input_photo_diode, &gpio_num, NULL);
}

static void gpio_task_input_photo_diode(void *arg) {
	uint32_t io_num;
	while (1) {
		if (xQueueReceive(gpio_event_queue_input_photo_diode, &io_num, portMAX_DELAY)) {
			// TODO: Logic implementation
			ESP_LOGI(TAG, "GPIO[%d] intr, value: %d\n", io_num, gpio_get_level(io_num));
		}
	}
}

/**
 * Input pin connected to ground
 * TODO Think about external pulldown receiver
 */

void init_output_pump() {
	gpio_config_t io_config;
	io_config.intr_type = GPIO_INTR_DISABLE;
	io_config.pin_bit_mask = (1ULL << PUMP_D0_OUTPUT);
	io_config.mode = GPIO_MODE_OUTPUT;
	io_config.pull_down_en = 1;
	gpio_config(&io_config);
}

void init_input_moisture_buttons() {
	gpio_config_t io_config;
	io_config.intr_type = GPIO_INTR_POSEDGE;
	io_config.pin_bit_mask = ((1ULL << LBUTTON_D1_INPUT) | (1ULL << RBUTTON_D5_INPUT ));
	io_config.mode = GPIO_MODE_INPUT;
	io_config.pull_down_en = 1;

	// Interrupt

	// Individual interrupt edge
	// gpio_set_intr_type(LBUTTON_D1_INPUT, GPIO_INTR_POSEDGE);
	// gpio_set_intr_type(RBUTTON_D5_INPUT, GPIO_INTR_NEGEDGE);

	gpio_event_queue_input_moisture_buttons = xQueueCreate(10, sizeof(uint32_t));
	xTaskCreate(gpio_task_input_moisture_buttons, "gpio_task_input_moisture_buttons", 2048, NULL, 10, NULL);

	gpio_isr_handler_add(LBUTTON_D1_INPUT, gpio_isr_handler_input_moisture_buttons, (void *) LBUTTON_D1_INPUT);
	gpio_isr_handler_add(RBUTTON_D5_INPUT, gpio_isr_handler_input_moisture_buttons, (void *) RBUTTON_D5_INPUT);

	gpio_config(&io_config);
}

void init_output_moisture_leds() {
	gpio_config_t io_config;
	io_config.intr_type = GPIO_INTR_DISABLE;
	io_config.pin_bit_mask = ((1ULL << LED_MOISTURE_1_D2_OUTPUT) | (1ULL << LED_MOISTURE_2_D3_OUTPUT ) | (1ULL << LED_MOISTURE_3_D4_OUTPUT));
	io_config.mode = GPIO_MODE_OUTPUT;
	io_config.pull_up_en = 1;

	gpio_config(&io_config);
}

void init_input_water_level_sensors() {
	gpio_config_t io_config;
	io_config.intr_type = GPIO_INTR_ANYEDGE;
	io_config.pin_bit_mask = ((1ULL << WATER_LEVEL_TOP_D6_INPUT) | (1ULL << WATER_LEVEL_BOTTOM_D7_INPUT ));
	io_config.mode = GPIO_MODE_INPUT;
	io_config.pull_down_en = 1;

	// Interrupt
	gpio_event_queue_input_water_level_sensors = xQueueCreate(10, sizeof(uint32_t));
	xTaskCreate(gpio_task_input_water_level_sensors, "gpio_task_input_water_level_sensors", 2048, NULL, 10, NULL);

	gpio_isr_handler_add(WATER_LEVEL_TOP_D6_INPUT, gpio_isr_handler_input_water_level_sensors, (void *) WATER_LEVEL_TOP_D6_INPUT);
	gpio_isr_handler_add(WATER_LEVEL_BOTTOM_D7_INPUT, gpio_isr_handler_input_water_level_sensors, (void *) WATER_LEVEL_BOTTOM_D7_INPUT);

	gpio_config(&io_config);
}

void init_output_water_level_leds() {
	gpio_config_t io_config;
	io_config.intr_type = GPIO_INTR_DISABLE;
	io_config.pin_bit_mask = ((1ULL << LED_WATER_LEVEL_TOP_D8_OUTPUT) | (1ULL << LED_WATER_LEVEL_BOTTOM_TX_OUTPUT ));
	io_config.mode = GPIO_MODE_OUTPUT;
	io_config.pull_up_en = 1;

	gpio_config(&io_config);
}

void init_input_photo_diode() {
	gpio_config_t io_config;
	io_config.intr_type = GPIO_INTR_ANYEDGE;
	io_config.pin_bit_mask = (1ULL << PHOTO_DIODE_RX_INPUT);
	io_config.mode = GPIO_MODE_INPUT;
	io_config.pull_up_en = 1;

	// Interrupt
	gpio_event_queue_input_photo_diode = xQueueCreate(10, sizeof(uint32_t));
	xTaskCreate(gpio_task_input_photo_diode, "gpio_task_input_photo_diode", 2048, NULL, 10, NULL);

	gpio_isr_handler_add(PHOTO_DIODE_RX_INPUT, gpio_isr_handler_input_photo_diode, (void *) PHOTO_DIODE_RX_INPUT);

	gpio_config(&io_config);
}

void init_input_moisture_sensor() {
	gpio_config_t io_config;
	io_config.intr_type = GPIO_INTR_DISABLE;

	// Mask error
	// io_config.pin_bit_mask = (1ULL << MOISTURE_SENSOR_A0_INPUT);

	io_config.mode = GPIO_MODE_INPUT;
	io_config.pull_up_en = 1;

	// No interrupt
	// => Retrieve moisture info every X seconds or minutes

	gpio_config(&io_config);
}

void init_gpio_output_example(gpio_config_t *io_config) {
	io_config->intr_type = GPIO_INTR_POSEDGE;
	io_config->mode = GPIO_MODE_OUTPUT;
	io_config->pin_bit_mask = ((1ULL<<GPIO_OUTPUT_LED) | (1ULL<<GPIO_OUTPUT_IO_1));
	io_config->pull_down_en = 0;
	io_config->pull_up_en = 0;
	gpio_config(io_config);

	gpio_event_queue_output_leds = xQueueCreate(10, sizeof(uint32_t));
	xTaskCreate(gpio_task_output_leds, "gpio_task_output_leds", 2048, NULL, 10, NULL);

	gpio_isr_handler_add(GPIO_OUTPUT_LED, gpio_isr_handler_output_leds, (void *) GPIO_OUTPUT_LED);
	//gpio_isr_handler_add(GPIO_OUTPUT_IO_1, gpio_isr_handler_output_leds, (void *) GPIO_OUTPUT_IO_1);

}

void blinkTask() {
	int cnt = 0;

	while (1) {
		ESP_LOGI(TAG, "cnt: %d\n", cnt++);
		vTaskDelay(1500 / portTICK_RATE_MS);
		gpio_set_level(GPIO_OUTPUT_LED, cnt % 2);
		gpio_set_level(GPIO_OUTPUT_IO_1, cnt % 2);
	}
}

void init_gpio() {
	// Implementation
	gpio_install_isr_service(0);

//	init_input_moisture_buttons();
//	init_input_water_level_sensors();
//	init_input_photo_diode();
//	init_input_moisture_sensor();
//	init_output_pump();
//	init_output_moisture_leds();
//	init_output_water_level_leds();

	// Example
	gpio_config_t io_conf;
	init_gpio_output_example(&io_conf);
	xTaskCreate(blinkTask, "blinkTask", 2048, NULL, 10, NULL);
}

