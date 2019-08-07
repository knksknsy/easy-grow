/*
 * easy_gpio.c
 * 
 * This file is responsible for setting up the nodeMCU's GPIOs,
 * initializing the interrupt service routines,
 * and setting up a timer for reading the analog soil moisture level.
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

// Queue Handler for ISR
static xQueueHandle gpio_event_queue = NULL;

/**
 * Installs the driver's GPIO ISR handler service.
 * This ISR handler will be called from an ISR.
 * The queue event will passed to the 'gpio_task' xTask.
 */
static void gpio_isr_handler(void *arg)
{
	uint32_t gpio_num = (uint32_t) arg;
	xQueueSendFromISR(gpio_event_queue, &gpio_num, NULL);
}

/**
 * Main handler for processing ISR queue events.
 */
static void gpio_task(void *arg)
{
	uint32_t io_num;
	while (1)
	{
		// Receive an item from a queue
		if (xQueueReceive(gpio_event_queue, &io_num, portMAX_DELAY))
		{
			ESP_LOGI(TAG, "GPIO[%d] intr, value: %d\n", io_num,
					gpio_get_level(io_num));

			switch (io_num)
			{
			// Moisture buttons pressed (up/down)
			case LBUTTON_D1_INPUT:
			case RBUTTON_D5_INPUT:
				moisture_button_handler(io_num);
				break;
			// Photo diodes state has changed (on/off)
			case PHOTO_DIODE_RX_INPUT:
				photo_diode_handler(io_num);
				break;
			default:
				break;
			}
		}
	}
}

/**
 * Initialization of water pump GPIO.
 * Input pin connected to ground.
 */
void init_pump_output()
{
	gpio_config_t io_config;
	io_config.intr_type = GPIO_INTR_DISABLE;
	io_config.pin_bit_mask = (1ULL << PUMP_D0_OUTPUT);
	io_config.mode = GPIO_MODE_OUTPUT;

	// Set GPIO level to 0 in order to prevent activation of GPIO after boot
	gpio_set_level(PUMP_D0_OUTPUT, 0);

	gpio_config(&io_config);
}

/**
 * Initialization of pressbuttons GPIOs for setting the desired moisture level.
 */
void init_moisture_buttons_input()
{
	gpio_config_t io_config;
	// Interrupt is triggered at any edge
	io_config.intr_type = GPIO_INTR_ANYEDGE;
	io_config.pin_bit_mask = ((1ULL << LBUTTON_D1_INPUT)
			| (1ULL << RBUTTON_D5_INPUT));
	io_config.mode = GPIO_MODE_INPUT;

	// Hook ISR handlers for specific GPIO pin
	gpio_isr_handler_add(LBUTTON_D1_INPUT, gpio_isr_handler,
			(void *) LBUTTON_D1_INPUT);
	gpio_isr_handler_add(RBUTTON_D5_INPUT, gpio_isr_handler,
			(void *) RBUTTON_D5_INPUT);

	gpio_config(&io_config);
}

/**
 * Initialization of LED GPIOs for signaling the current moisture level.
 */
void init_moisture_leds_output()
{
	gpio_config_t io_config;
	io_config.intr_type = GPIO_INTR_DISABLE;
	io_config.pin_bit_mask = ((1ULL << LED_MOISTURE_1_D2_OUTPUT)
			| (1ULL << LED_MOISTURE_2_D3_OUTPUT)
			| (1ULL << LED_MOISTURE_3_D4_OUTPUT));
	io_config.mode = GPIO_MODE_OUTPUT;

	// Set GPIO levels to 0 in order to prevent activation of GPIOs after boot
	gpio_set_level(LED_MOISTURE_1_D2_OUTPUT, 0);
	gpio_set_level(LED_MOISTURE_2_D3_OUTPUT, 0);
	gpio_set_level(LED_MOISTURE_3_D4_OUTPUT, 0);

	gpio_config(&io_config);
}

/**
 * Initialization of water sensor GPIOs.
 */
void init_water_level_sensors_input()
{
	gpio_config_t io_config;
	io_config.intr_type = GPIO_INTR_ANYEDGE;
	io_config.pin_bit_mask = ((1ULL << WATER_LEVEL_TOP_D6_INPUT)
			| (1ULL << WATER_LEVEL_BOTTOM_D7_INPUT));
	io_config.mode = GPIO_MODE_INPUT;

	gpio_config(&io_config);
}

/**
 * Initialization of LED GPIOs for showing the current water level.
 */
void init_water_level_leds_output()
{
	gpio_config_t io_config;
	io_config.intr_type = GPIO_INTR_DISABLE;
	io_config.pin_bit_mask = ((1ULL << LED_WATER_LEVEL_TOP_D8_OUTPUT)
			| (1ULL << LED_WATER_LEVEL_BOTTOM_TX_OUTPUT));
	io_config.mode = GPIO_MODE_OUTPUT;

	// Set GPIO level to 0 in order to prevent activation of GPIO after boot
	gpio_set_level(LED_WATER_LEVEL_TOP_D8_OUTPUT, 0);
	gpio_set_level(LED_WATER_LEVEL_BOTTOM_TX_OUTPUT, 0);

	gpio_config(&io_config);
}

/**
 * Initialization of photo diode's GPIO.
 */
void init_photo_diode_input()
{
	gpio_config_t io_config;
	io_config.intr_type = GPIO_INTR_ANYEDGE;
	io_config.pin_bit_mask = (1ULL << PHOTO_DIODE_RX_INPUT);
	io_config.mode = GPIO_MODE_INPUT;

	// Hook ISR handlers for specific GPIO pin
	gpio_isr_handler_add(PHOTO_DIODE_RX_INPUT, gpio_isr_handler,
			(void *) PHOTO_DIODE_RX_INPUT);

	gpio_config(&io_config);
}

/**
 * Initialization of a timer. This timer will read the soil moisture level repeatedly for the defined MOISTURE_READ_INTERVAL.
 */
void init_adc_timer()
{
	/* Create timer */
	const esp_timer_create_args_t adc_timer_args =
	{ .callback = &read_moisture_level, .name = "adc_timer" };

	esp_timer_handle_t adc_timer;
	ESP_ERROR_CHECK(esp_timer_create(&adc_timer_args, &adc_timer));

	/* Start timer */
	ESP_ERROR_CHECK(esp_timer_start_periodic(adc_timer, MOISTURE_READ_INTERVAL));
	ESP_LOGI(TAG, "Started timer, time since boot: (%d)",
			(int32_t) esp_timer_get_time());
}

/**
 * Initialization of soil moisture sensor's analog GPIO.
 */
void init_moisture_sensor_adc_input()
{
	adc_config_t adc_config;

	// Depend on menuconfig -> Component config -> PHY -> vdd33_const value
	// When measuring system voltage(ADC_READ_VDD_MODE), vdd33_const must be set to 255.
	adc_config.mode = ADC_READ_TOUT_MODE;
	// ADC sample collection clock = 80MHz/clk_div = 10MHz
	adc_config.clk_div = 8;
	ESP_ERROR_CHECK(adc_init(&adc_config));

	// Initialization of the timer
	init_adc_timer();
}

/**
 * Initialize Interrupt Service Routine.
 */
void init_isr()
{
	// Install GPIO ISR service
	gpio_install_isr_service(0);
	// Create a queue to handle GPIO event from ISR
	gpio_event_queue = xQueueCreate(10, sizeof(uint32_t));
	// Create and start xTask
	xTaskCreate(gpio_task, "gpio_task", 4096, NULL, 10, NULL);
}

/**
 * Entrance method for initializing nodeMCU's GPIOs.
 */
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
