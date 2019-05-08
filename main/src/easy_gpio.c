/*
 * easy_gpio.c
 *
 *  Created on: 7 May 2019
 *      Author: m.bilge
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

static const char *TAG = "main";

/**
 *
 *  GPIO class to read sensors and do stuff
 *
 *
 */

#define LED_PIN 4

#define GPIO_OUTPUT_IO_1    16
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_IO_1)| (1ULL<<LED_PIN))

#define GPIO_INPUT_WATER_LEVEL_ONE     4
#define GPIO_INPUT_WATER_LEVEL_TWO     5
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_WATER_LEVEL_ONE) | (1ULL<<GPIO_INPUT_WATER_LEVEL_TWO))

static xQueueHandle gpio_evt_queue = NULL;



/**
 *
 * Input pin connected to ground
 * TODO Think about external pulldown receiver
 */
static void gpio_input_received(void *arg)
{
    uint32_t io_num;

    for (;;) {
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            ESP_LOGI(TAG, "GPIO[%d] intr, val: %d\n", io_num, gpio_get_level(io_num));
        }
    }
}

static void gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

void gpio_task(int pin, int value) {
	gpio_set_level(pin, value);
}

void checkStates() {
	//readWaterlevel();
}



void blinkTask() {
	bool isHigh = false;

	while (1) {
		vTaskDelay(1000 / portTICK_RATE_MS);
		gpio_set_level(LED_PIN, isHigh ? 1 : 0);
		isHigh = !isHigh;
		gpio_set_level(GPIO_OUTPUT_IO_1, isHigh ? 1 : 0);
		ESP_LOGI(TAG, "Blink Task called\n");
	}
}

void init_gpio_output(gpio_config_t io_conf) {
	io_conf.intr_type = GPIO_INTR_DISABLE;
	io_conf.mode = GPIO_MODE_OUTPUT;
	io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
	io_conf.pull_down_en = 0;
	io_conf.pull_up_en = 0;
	gpio_config(&io_conf);
}

void init_gpio_input(gpio_config_t io_conf) {
	    io_conf.intr_type = GPIO_INTR_POSEDGE;
	    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
	    io_conf.mode = GPIO_MODE_INPUT;
	    io_conf.pull_up_en = 1;
	    gpio_config(&io_conf);

	    gpio_set_intr_type(GPIO_INPUT_WATER_LEVEL_ONE, GPIO_INTR_ANYEDGE);
	    gpio_set_intr_type(GPIO_INPUT_WATER_LEVEL_TWO, GPIO_INTR_ANYEDGE);

	    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
	    xTaskCreate(gpio_input_received, "gpio_input_received", 2048, NULL, 10, NULL);

	    gpio_install_isr_service(0);
	    gpio_isr_handler_add(GPIO_INPUT_WATER_LEVEL_ONE, gpio_isr_handler, (void *) GPIO_INPUT_WATER_LEVEL_ONE);
	    gpio_isr_handler_add(GPIO_INPUT_WATER_LEVEL_TWO, gpio_isr_handler, (void *) GPIO_INPUT_WATER_LEVEL_TWO);

}

void init_gpio(void) {
	gpio_config_t io_conf;
	void init_gpio_output(io_conf);
	void init_gpio_input(io_conf);
}

