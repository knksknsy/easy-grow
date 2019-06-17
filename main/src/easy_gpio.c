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
#include "driver/hw_timer.h"

#include "esp_log.h"
#include "esp_system.h"
#include "easy_gpio.h"
#include "esp_timer.h"
#include "esp_sleep.h"

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

static xQueueHandle gpio_event_queue_input_moisture_button_l = NULL;
static xQueueHandle gpio_event_queue_input_moisture_button_r = NULL;
static xQueueHandle gpio_event_queue_input_water_level_sensors = NULL;
static xQueueHandle gpio_event_queue_input_photo_diode = NULL;

static ButtonStates lbutton_states;
static ButtonStates rbutton_states;

void setMoistureLevel(int *level)
{
	uint32_t color = 0x00FF0000;
	ws2812_set_many(LED_MOISTURE_2_D3_OUTPUT, &color, (size_t)level);
}

static ButtonStates delay_debounce(ButtonStates button_state, int button_gpio)
{
	/* if pressed */
	if (gpio_get_level(button_gpio))
	{
		if (button_state == PRESS)
		{
			button_state = DOWN;
		}
		if (button_state == UP)
		{
			vTaskDelay(25 / portTICK_RATE_MS);
			if (gpio_get_level(button_gpio) == 1)
			{
				button_state = PRESS;
			}
		}
	}
	/* if not pressed */
	else
	{
		if (button_state == RELEASE)
		{
			button_state = UP;
		}
		if (button_state == DOWN)
		{
			if (gpio_get_level(button_gpio) == 0)
			{
				vTaskDelay(25 / portTICK_RATE_MS);
				if (gpio_get_level(button_gpio) == 0)
				{
					button_state = RELEASE;
				}
			}
		}
	}
	return button_state;
}

static void gpio_isr_handler_input_moisture_button_l(void *arg)
{
	uint32_t gpio_num = (uint32_t)arg;
	xQueueSendFromISR(gpio_event_queue_input_moisture_button_l, &gpio_num, NULL);
}

static void gpio_task_input_moisture_button_l(void *arg)
{
	uint32_t io_num;
	while (1)
	{
		if (xQueueReceive(gpio_event_queue_input_moisture_button_l, &io_num, portMAX_DELAY))
		{
			lbutton_states = delay_debounce(lbutton_states, LBUTTON_D1_INPUT);
			ESP_LOGI(TAG, "l_button_states: %d", lbutton_states);

			if (lbutton_states == PRESS)
			{
				int LED1 = gpio_get_level(LED_MOISTURE_1_D2_OUTPUT);
				int LED2 = gpio_get_level(LED_MOISTURE_2_D3_OUTPUT);
				int LED3 = gpio_get_level(LED_MOISTURE_3_D4_OUTPUT);

				if (!LED1 && !LED2 && !LED3)
				{
					LED1 = 1, LED2 = 1, LED3 = 1;
				}
				else if (LED1 && LED2 && LED3)
				{
					LED1 = 1, LED2 = 1, LED3 = 0;
				}
				else if (LED1 && LED2 && !LED3)
				{
					LED1 = 1, LED2 = 0, LED3 = 0;
				}
				else if (LED1 && !LED2 && !LED3)
				{
					LED1 = 0, LED2 = 0, LED3 = 0;
				}

				gpio_set_level(LED_MOISTURE_1_D2_OUTPUT, LED1);
				gpio_set_level(LED_MOISTURE_2_D3_OUTPUT, LED2);
				gpio_set_level(LED_MOISTURE_3_D4_OUTPUT, LED3);
			}
		}
	}
}

static void gpio_isr_handler_input_moisture_button_r(void *arg)
{
	uint32_t gpio_num = (uint32_t)arg;
	xQueueSendFromISR(gpio_event_queue_input_moisture_button_r, &gpio_num, NULL);
}

static void gpio_task_input_moisture_button_r(void *arg)
{
	uint32_t io_num;
	while (1)
	{
		if (xQueueReceive(gpio_event_queue_input_moisture_button_r, &io_num, portMAX_DELAY))
		{
			rbutton_states = delay_debounce(rbutton_states, RBUTTON_D5_INPUT);
			ESP_LOGI(TAG, "r_button_states: %d", rbutton_states);

			if (rbutton_states == PRESS)
			{
				int LED1 = gpio_get_level(LED_MOISTURE_1_D2_OUTPUT);
				int LED2 = gpio_get_level(LED_MOISTURE_2_D3_OUTPUT);
				int LED3 = gpio_get_level(LED_MOISTURE_3_D4_OUTPUT);

				if (!LED1 && !LED2 && !LED3)
				{
					LED1 = 1, LED2 = 0, LED3 = 0;
				}
				else if (LED1 && !LED2 && !LED3)
				{
					LED1 = 1, LED2 = 1, LED3 = 0;
				}
				else if (LED1 && LED2 && !LED3)
				{
					LED1 = 1, LED2 = 1, LED3 = 1;
				}
				else if (LED1 && LED2 && LED3)
				{
					LED1 = 0, LED2 = 0, LED3 = 0;
				}

				gpio_set_level(LED_MOISTURE_1_D2_OUTPUT, LED1);
				gpio_set_level(LED_MOISTURE_2_D3_OUTPUT, LED2);
				gpio_set_level(LED_MOISTURE_3_D4_OUTPUT, LED3);
			}
		}
	}
}

static void gpio_isr_handler_input_water_level_sensors(void *arg)
{
	uint32_t gpio_num = (uint32_t)arg;
	xQueueSendFromISR(gpio_event_queue_input_water_level_sensors, &gpio_num, NULL);
}

static void gpio_task_input_water_level_sensors(void *arg)
{
	uint32_t io_num;
	while (1)
	{
		if (xQueueReceive(gpio_event_queue_input_water_level_sensors, &io_num, portMAX_DELAY))
		{
			// TODO: Logic implementation
			ESP_LOGI(TAG, "GPIO[%d] intr, value: %d\n", io_num, gpio_get_level(io_num));
		}
	}
}

static void gpio_isr_handler_input_photo_diode(void *arg)
{
	uint32_t gpio_num = (uint32_t)arg;
	xQueueSendFromISR(gpio_event_queue_input_photo_diode, &gpio_num, NULL);
}

static void gpio_task_input_photo_diode(void *arg)
{
	uint32_t io_num;
	while (1)
	{
		if (xQueueReceive(gpio_event_queue_input_photo_diode, &io_num, portMAX_DELAY))
		{
			// TODO: Logic implementation
			ESP_LOGI(TAG, "GPIO[%d] intr, value: %d\n", io_num, gpio_get_level(io_num));
		}
	}
}

/**
 * Input pin connected to ground
 * TODO Think about external pulldown receiver
 */
void init_output_pump()
{
	gpio_config_t io_config;
	io_config.intr_type = GPIO_INTR_DISABLE;
	io_config.pin_bit_mask = (1ULL << PUMP_D0_OUTPUT);
	io_config.mode = GPIO_MODE_OUTPUT;
	io_config.pull_down_en = 1;
	gpio_config(&io_config);
}

void init_input_moisture_button_l()
{
	gpio_config_t io_config;
	io_config.intr_type = GPIO_INTR_ANYEDGE;
	io_config.pin_bit_mask = (1ULL << LBUTTON_D1_INPUT);
	io_config.mode = GPIO_MODE_INPUT;
	io_config.pull_down_en = 1;

	// Interrupt

	// Individual interrupt edge
	// gpio_set_intr_type(LBUTTON_D1_INPUT, GPIO_INTR_POSEDGE);
	// gpio_set_intr_type(RBUTTON_D5_INPUT, GPIO_INTR_NEGEDGE);

	gpio_event_queue_input_moisture_button_l = xQueueCreate(10, sizeof(uint32_t));
	xTaskCreate(gpio_task_input_moisture_button_l, "gpio_task_input_moisture_button_l", 2048, NULL, 10, NULL);

	gpio_isr_handler_add(LBUTTON_D1_INPUT, gpio_isr_handler_input_moisture_button_l, (void *)LBUTTON_D1_INPUT);

	gpio_config(&io_config);
}

void init_input_moisture_button_r()
{
	gpio_config_t io_config;
	io_config.intr_type = GPIO_INTR_ANYEDGE;
	io_config.pin_bit_mask = (1ULL << RBUTTON_D5_INPUT);
	io_config.mode = GPIO_MODE_INPUT;
	io_config.pull_down_en = 1;

	// Interrupt

	// Individual interrupt edge
	// gpio_set_intr_type(LBUTTON_D1_INPUT, GPIO_INTR_POSEDGE);
	// gpio_set_intr_type(RBUTTON_D5_INPUT, GPIO_INTR_NEGEDGE);

	gpio_event_queue_input_moisture_button_r = xQueueCreate(10, sizeof(uint32_t));
	xTaskCreate(gpio_task_input_moisture_button_r, "gpio_task_input_moisture_button_r", 2048, NULL, 10, NULL);

	gpio_isr_handler_add(RBUTTON_D5_INPUT, gpio_isr_handler_input_moisture_button_r, (void *)RBUTTON_D5_INPUT);

	gpio_config(&io_config);
}

void init_output_moisture_leds()
{
	gpio_config_t io_config;
	io_config.intr_type = GPIO_INTR_DISABLE;
	io_config.pin_bit_mask = ((1ULL << LED_MOISTURE_1_D2_OUTPUT) | (1ULL << LED_MOISTURE_2_D3_OUTPUT) | (1ULL << LED_MOISTURE_3_D4_OUTPUT));
	io_config.mode = GPIO_MODE_OUTPUT;
	io_config.pull_up_en = 1;

	gpio_config(&io_config);
}

void init_input_water_level_sensors()
{
	gpio_config_t io_config;
	io_config.intr_type = GPIO_INTR_ANYEDGE;
	io_config.pin_bit_mask = ((1ULL << WATER_LEVEL_TOP_D6_INPUT) | (1ULL << WATER_LEVEL_BOTTOM_D7_INPUT));
	io_config.mode = GPIO_MODE_INPUT;
	io_config.pull_down_en = 1;

	// Interrupt
	gpio_event_queue_input_water_level_sensors = xQueueCreate(10, sizeof(uint32_t));
	xTaskCreate(gpio_task_input_water_level_sensors, "gpio_task_input_water_level_sensors", 2048, NULL, 10, NULL);

	gpio_isr_handler_add(WATER_LEVEL_TOP_D6_INPUT, gpio_isr_handler_input_water_level_sensors, (void *)WATER_LEVEL_TOP_D6_INPUT);
	gpio_isr_handler_add(WATER_LEVEL_BOTTOM_D7_INPUT, gpio_isr_handler_input_water_level_sensors, (void *)WATER_LEVEL_BOTTOM_D7_INPUT);

	gpio_config(&io_config);
}

void init_output_water_level_leds()
{
	gpio_config_t io_config;
	io_config.intr_type = GPIO_INTR_DISABLE;
	io_config.pin_bit_mask = ((1ULL << LED_WATER_LEVEL_TOP_D8_OUTPUT) | (1ULL << LED_WATER_LEVEL_BOTTOM_TX_OUTPUT));
	io_config.mode = GPIO_MODE_OUTPUT;
	io_config.pull_up_en = 1;

	gpio_config(&io_config);
}

void init_input_photo_diode()
{
	gpio_config_t io_config;
	io_config.intr_type = GPIO_INTR_ANYEDGE;
	io_config.pin_bit_mask = (1ULL << PHOTO_DIODE_RX_INPUT);
	io_config.mode = GPIO_MODE_INPUT;
	io_config.pull_up_en = 1;

	// Interrupt
	gpio_event_queue_input_photo_diode = xQueueCreate(10, sizeof(uint32_t));
	xTaskCreate(gpio_task_input_photo_diode, "gpio_task_input_photo_diode", 2048, NULL, 10, NULL);

	gpio_isr_handler_add(PHOTO_DIODE_RX_INPUT, gpio_isr_handler_input_photo_diode, (void *)PHOTO_DIODE_RX_INPUT);

	gpio_config(&io_config);
}

void init_input_moisture_sensor()
{
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

void init_gpio()
{
	gpio_install_isr_service(0);

	init_input_moisture_button_l();
	init_input_moisture_button_r();
	//	init_input_water_level_sensors();
	//	init_input_photo_diode();
	//	init_input_moisture_sensor();
	//	init_output_pump();
	init_output_moisture_leds();
	//	init_output_water_level_leds();
}
