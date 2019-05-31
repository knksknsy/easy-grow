/*
 * easy_example.c
 *
 *  Created on:	31 May 2019
 *	Author:		Kaan Keskinsoy
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"

#include <esp_log.h>
#include <esp_system.h>
#include "easy_example.h"

static const char *TAG = "EXAMPLE";

/**
 * GPIO definitions
 */
#define GPIO_OUTPUT_LED_1			2
#define GPIO_OUTPUT_LED_2   		16

static xQueueHandle gpio_event_queue_output_leds = NULL;

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

void init_gpio_output_leds() {
	gpio_config_t io_config;
	io_config.intr_type = GPIO_INTR_POSEDGE;
	io_config.mode = GPIO_MODE_OUTPUT;
	io_config.pin_bit_mask = ((1ULL<<GPIO_OUTPUT_LED_1) | (1ULL<<GPIO_OUTPUT_LED_2));
	io_config.pull_down_en = 0;
	io_config.pull_up_en = 0;
	gpio_config(&io_config);

	gpio_event_queue_output_leds = xQueueCreate(10, sizeof(uint32_t));
	xTaskCreate(gpio_task_output_leds, "gpio_task_output_leds", 2048, NULL, 10, NULL);

	gpio_isr_handler_add(GPIO_OUTPUT_LED_1, gpio_isr_handler_output_leds, (void *) GPIO_OUTPUT_LED_1);
	//gpio_isr_handler_add(GPIO_OUTPUT_LED_2, gpio_isr_handler_output_leds, (void *) GPIO_OUTPUT_LED_2);
}

void blinkTask() {
	int cnt = 0;

	while (1) {
		ESP_LOGI(TAG, "cnt: %d\n", cnt++);
		vTaskDelay(1500 / portTICK_RATE_MS);
		gpio_set_level(GPIO_OUTPUT_LED_1, cnt % 2);
		gpio_set_level(GPIO_OUTPUT_LED_2, cnt % 2);
	}
}

void init_example() {
	gpio_install_isr_service(0);
	init_gpio_output_leds();
	xTaskCreate(blinkTask, "blinkTask", 2048, NULL, 10, NULL);
}
