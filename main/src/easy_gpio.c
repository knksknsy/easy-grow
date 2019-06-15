/*
 * easy_gpio.c
 *
 *  Created on:	7 May 2019
 *	Author:		m.bilge, Kaan Keskinsoy
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "easy_gpio.h"
#include "easy_controller.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"
#include "driver/adc.h"

#include "esp_timer.h"
#include "esp_sleep.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "assert.h"
#include "sdkconfig.h"

static const char *TAG = "GPIO";

static xQueueHandle gpio_event_queue = NULL;

static void gpio_isr_handler(void *arg)
{
	uint32_t gpio_num = (uint32_t) arg;
	xQueueSendFromISR(gpio_event_queue, &gpio_num, NULL);
}

static void gpio_task(void *arg)
{
	uint32_t io_num;
	while (1)
	{
		if (xQueueReceive(gpio_event_queue, &io_num, portMAX_DELAY))
		{
			ESP_LOGI(TAG, "GPIO[%d] intr, value: %d\n", io_num,
					gpio_get_level(io_num));

			switch (io_num)
			{
			case LBUTTON_D1_INPUT:
			case RBUTTON_D5_INPUT:
				moisture_button_handler(io_num);
				break;
			case PHOTO_DIODE_RX_INPUT:
				photo_diode_handler(io_num);
				break;
			case WATER_LEVEL_TOP_D6_INPUT:
			case WATER_LEVEL_BOTTOM_D7_INPUT:
				water_level_handler(io_num);
				break;
			default:
				break;
			}
		}
	}
}

/**
 * Input pin connected to ground
 * TODO Think about external pulldown receiver
 */
void init_pump_output()
{
	gpio_config_t io_config;
	io_config.intr_type = GPIO_INTR_DISABLE;
	io_config.pin_bit_mask = (1ULL << PUMP_D0_OUTPUT);
	io_config.mode = GPIO_MODE_OUTPUT;
	io_config.pull_down_en = 1;

	gpio_set_level(PUMP_D0_OUTPUT, 0);

	gpio_config(&io_config);
}

void init_moisture_buttons_input()
{
	gpio_config_t io_config;
	io_config.intr_type = GPIO_INTR_ANYEDGE;
	io_config.pin_bit_mask = ((1ULL << LBUTTON_D1_INPUT)
			| (1ULL << RBUTTON_D5_INPUT));
	io_config.mode = GPIO_MODE_INPUT;
	io_config.pull_down_en = 1;

	// Individual interrupt edge
	// gpio_set_intr_type(LBUTTON_D1_INPUT, GPIO_INTR_POSEDGE);
	// gpio_set_intr_type(RBUTTON_D5_INPUT, GPIO_INTR_NEGEDGE);

	gpio_isr_handler_add(LBUTTON_D1_INPUT, gpio_isr_handler,
			(void *) LBUTTON_D1_INPUT);
	gpio_isr_handler_add(RBUTTON_D5_INPUT, gpio_isr_handler,
			(void *) RBUTTON_D5_INPUT);

	gpio_config(&io_config);
}

void init_moisture_leds_output()
{
	gpio_config_t io_config;
	io_config.intr_type = GPIO_INTR_DISABLE;
	io_config.pin_bit_mask = ((1ULL << LED_MOISTURE_1_D2_OUTPUT)
			| (1ULL << LED_MOISTURE_2_D3_OUTPUT)
			| (1ULL << LED_MOISTURE_3_D4_OUTPUT));
	io_config.mode = GPIO_MODE_OUTPUT;
	io_config.pull_up_en = 1;

	gpio_set_level(LED_MOISTURE_1_D2_OUTPUT, 0);
	gpio_set_level(LED_MOISTURE_2_D3_OUTPUT, 0);
	gpio_set_level(LED_MOISTURE_3_D4_OUTPUT, 0);

	gpio_config(&io_config);
}

void init_water_level_sensors_input()
{
	gpio_config_t io_config;
	io_config.intr_type = GPIO_INTR_ANYEDGE;
	io_config.pin_bit_mask = ((1ULL << WATER_LEVEL_TOP_D6_INPUT)
			| (1ULL << WATER_LEVEL_BOTTOM_D7_INPUT));
	io_config.mode = GPIO_MODE_INPUT;
	// io_config.pull_down_en = 1;

	gpio_isr_handler_add(WATER_LEVEL_TOP_D6_INPUT, gpio_isr_handler,
			(void *) WATER_LEVEL_TOP_D6_INPUT);
	gpio_isr_handler_add(WATER_LEVEL_BOTTOM_D7_INPUT, gpio_isr_handler,
			(void *) WATER_LEVEL_BOTTOM_D7_INPUT);

	gpio_config(&io_config);
}

void init_water_level_leds_output()
{
	gpio_config_t io_config;
	io_config.intr_type = GPIO_INTR_DISABLE;
	io_config.pin_bit_mask = ((1ULL << LED_WATER_LEVEL_TOP_D8_OUTPUT)
			| (1ULL << LED_WATER_LEVEL_BOTTOM_TX_OUTPUT));
	io_config.mode = GPIO_MODE_OUTPUT;
	io_config.pull_up_en = 1;

	gpio_set_level(LED_WATER_LEVEL_TOP_D8_OUTPUT, 0);
	gpio_set_level(LED_WATER_LEVEL_BOTTOM_TX_OUTPUT, 0);

	gpio_config(&io_config);
}

void init_photo_diode_input()
{
	gpio_config_t io_config;
	io_config.intr_type = GPIO_INTR_ANYEDGE;
	io_config.pin_bit_mask = (1ULL << PHOTO_DIODE_RX_INPUT);
	io_config.mode = GPIO_MODE_INPUT;
	io_config.pull_down_en = 1;

	gpio_isr_handler_add(PHOTO_DIODE_RX_INPUT, gpio_isr_handler,
			(void *) PHOTO_DIODE_RX_INPUT);

	gpio_config(&io_config);
}

static void adc_timer_callback(void *arg)
{
	int64_t time_since_boot = esp_timer_get_time();
	ESP_LOGI(TAG, "ADC timer called, time since boot: (%d)",
			(int32_t) time_since_boot);

	int x;
	uint8_t data_size = 20;
	uint16_t adc_data[data_size];
	if (ESP_OK == adc_read(&adc_data[0]))
	{
		ESP_LOGI(TAG, "adc read: %d\r\n", adc_data[0]);
	}

	ESP_LOGI(TAG, "adc read fast:\r\n");

	if (ESP_OK == adc_read_fast(adc_data, data_size))
	{
		for (x = 0; x < data_size; x++)
		{
			printf("%d\n", adc_data[x]);
		}
	}
}

void init_adc_timer()
{
	/* Create timer */
	const esp_timer_create_args_t adc_timer_args =
	{ .callback = &adc_timer_callback, .name = "adc_timer" };

	esp_timer_handle_t adc_timer;
	ESP_ERROR_CHECK(esp_timer_create(&adc_timer_args, &adc_timer));

	/* Start timer 5s interval */
	ESP_ERROR_CHECK(esp_timer_start_periodic(adc_timer, 5000000));
	ESP_LOGI(TAG, "Started timer, time since boot: (%d)",
			(int32_t) esp_timer_get_time());
}

void init_moisture_sensor_adc_input()
{
	adc_config_t adc_config;

	// Depend on menuconfig -> Component config -> PHY -> vdd33_const value
	// When measuring system voltage(ADC_READ_VDD_MODE), vdd33_const must be set to 255.
	adc_config.mode = ADC_READ_TOUT_MODE;
	// ADC sample collection clock = 80MHz/clk_div = 10MHz
	adc_config.clk_div = 8;
	ESP_ERROR_CHECK(adc_init(&adc_config));

	init_adc_timer();
}

void init_isr()
{
	gpio_install_isr_service(0);
	gpio_event_queue = xQueueCreate(10, sizeof(uint32_t));
	xTaskCreate(gpio_task, "gpio_task", 8000, NULL, 10, NULL);
}

void init_gpio()
{
	init_isr();

	init_moisture_buttons_input();
	init_moisture_leds_output();
	init_water_level_sensors_input();
	init_pump_output();
	init_moisture_sensor_adc_input();
	/* Disable RX and TX GPIOs for monitoring */
	init_photo_diode_input();
	init_water_level_leds_output();
	/* // */
}
