/*
 * easy_controller.c
 *
 *  Created on: 3 Jun 2019
 *      Author: Kaan Keskinsoy
 */

#include <stdint.h>

#include "easy_controller.h"
#include "easy_gpio.h"
#include "easy_debouncer.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"

#include "esp_log.h"
#include "esp_system.h"
#include "sdkconfig.h"

//#include "ws2812.h"

static const char *TAG = "CONTROLLER";
// static int water_led_task_initialized = 0;

void moisture_button_handler(uint32_t io_num)
{
	/* Set ideal moisture level via hardware buttons. Sets moisture LED outputs for the configured mositure level. */

	static ButtonStates lbutton_states;
	static ButtonStates rbutton_states;

	int LED1 = gpio_get_level(LED_MOISTURE_1_D2_OUTPUT);
	int LED2 = gpio_get_level(LED_MOISTURE_2_D3_OUTPUT);
	int LED3 = gpio_get_level(LED_MOISTURE_3_D4_OUTPUT);

	/* Decrease moisture */
	if (io_num == LBUTTON_D1_INPUT)
	{

		lbutton_states = delay_debounce(lbutton_states, LBUTTON_D1_INPUT);
		ESP_LOGI(TAG, "l_button_states: %d", lbutton_states);

		if (lbutton_states == PRESS)
		{
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
		}
	}
	/* Increase moisture */
	else if (io_num == RBUTTON_D5_INPUT)
	{
		rbutton_states = delay_debounce(rbutton_states, RBUTTON_D5_INPUT);
		ESP_LOGI(TAG, "r_button_states: %d", rbutton_states);

		if (rbutton_states == PRESS)
		{
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
		}
	}
	if (lbutton_states == PRESS || rbutton_states == PRESS)
	{
		gpio_set_level(LED_MOISTURE_1_D2_OUTPUT, LED1);
		gpio_set_level(LED_MOISTURE_2_D3_OUTPUT, LED2);
		gpio_set_level(LED_MOISTURE_3_D4_OUTPUT, LED3);
	}
}

void moisture_leds_handler(uint8_t level)
{
	/* Set moisture LED outputs for the configured moisture level (server-side configuration of moisture level). */
}

void set_moisture_level(uint8_t level)
{
	/* Set the ideal value for moisture. Pump waters plant until its ideal moisture level is reached. */
}

uint8_t get_moisture_level()
{
	/* Read ADC input fast and return the average moisture level */
	return NULL;
}

void photo_diode_handler(uint32_t io_num)
{
	int value = gpio_get_level(io_num);
	ESP_LOGI(TAG, "photo diode: %d", value);
	/* Read photo diode input and count the hours of sun for the current day */
}

uint8_t get_hours_of_sun()
{
	/* Return hours of sun registered for the current day */
	return NULL;
}

// static void water_low_task(void *arg)
// {
// 	int counter = 0;
// 	while (1)
// 	{
// 		counter++;
// 		gpio_set_level(LED_WATER_LEVEL_TOP_D8_OUTPUT, counter % 2);
// 		gpio_set_level(LED_WATER_LEVEL_BOTTOM_TX_OUTPUT, counter % 2);
// 		ESP_LOGI(TAG, "WATER LEVEL LEDS: (%d|%d)", counter % 2, counter % 2);
// 		vTaskDelay(1000 / portTICK_RATE_MS);
// 	}
// }

void water_level_handler(uint32_t io_num)
{
	int WATER_TOP = gpio_get_level(WATER_LEVEL_TOP_D6_INPUT) == 0 ? 1 : 0;
	int WATER_BOTTOM = gpio_get_level(WATER_LEVEL_BOTTOM_D7_INPUT) == 0 ? 1 : 0;

	int LED_TOP = gpio_get_level(LED_WATER_LEVEL_TOP_D8_OUTPUT);
	int LED_BOTTOM = gpio_get_level(LED_WATER_LEVEL_BOTTOM_TX_OUTPUT);

	// TaskHandle_t blink_handle;

	if (WATER_TOP && WATER_BOTTOM)
	{
		// vTaskSuspend(blink_handle);

		LED_BOTTOM = 1, LED_TOP = 1;
		ESP_LOGI(TAG, "WATER LEVEL LEDS: (%d|%d)", LED_BOTTOM, LED_TOP);
		gpio_set_level(LED_WATER_LEVEL_TOP_D8_OUTPUT, LED_TOP);
		gpio_set_level(LED_WATER_LEVEL_BOTTOM_TX_OUTPUT, LED_BOTTOM);

		/* Activate water pump */
	}
	else if (!WATER_TOP && WATER_BOTTOM)
	{
		// vTaskSuspend(blink_handle);

		LED_BOTTOM = 1, LED_TOP = 0;
		ESP_LOGI(TAG, "WATER LEVEL LEDS: (%d|%d)", LED_BOTTOM, LED_TOP);
		gpio_set_level(LED_WATER_LEVEL_TOP_D8_OUTPUT, LED_TOP);
		gpio_set_level(LED_WATER_LEVEL_BOTTOM_TX_OUTPUT, LED_BOTTOM);

		/* Activate water pump */
	}
	else if (!WATER_TOP && !WATER_BOTTOM)
	{
		/* Create blinking LEDs task */
		// if (!water_led_task_initialized)
		// {
		// 	xTaskCreate(water_low_task, "water_low_task", 2048, NULL, 10, &blink_handle);
		// 	water_led_task_initialized = 1;
		// } else {
		// 	vTaskResume(blink_handle);
		// }

		/* Deactivate water pump */

		LED_BOTTOM = 0, LED_TOP = 0;
		ESP_LOGI(TAG, "WATER LEVEL LEDS: (%d|%d)", LED_BOTTOM, LED_TOP);
		gpio_set_level(LED_WATER_LEVEL_TOP_D8_OUTPUT, LED_TOP);
		gpio_set_level(LED_WATER_LEVEL_BOTTOM_TX_OUTPUT, LED_BOTTOM);
	}
}

WaterLevel get_water_level()
{
	/* Read water sensor inputs and return water level */
	return EMPTY;
}

void activate_pump(uint32_t ms)
{
	/* Water the plant as long as the provided milliseconds */
}

void deactivate_pump()
{
}

//void setMoistureLevel(int *level)
//{
//	uint32_t color = 0x00FF0000;
//	ws2812_set_many(LED_MOISTURE_2_D3_OUTPUT, &color, (size_t) level);
//}
