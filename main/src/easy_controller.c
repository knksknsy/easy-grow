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

#include "driver/gpio.h"

#include "esp_log.h"
#include "esp_system.h"
#include "sdkconfig.h"

//#include "ws2812.h"

static const char *TAG = "CONTROLLER";

void moisture_button_handler(uint32_t io_num)
{
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

}
void set_moisture_level(uint8_t level)
{

}
uint8_t get_moisture_level()
{
	return NULL;
}

void photo_diode_handler(uint32_t io_num)
{

}

uint8_t get_hours_of_sun()
{
	return NULL;
}

void water_level_handler(uint32_t io_num)
{
	int LED_TOP = gpio_get_level(LED_WATER_LEVEL_TOP_D8_OUTPUT);
	int LED_BOTTOM = gpio_get_level(LED_WATER_LEVEL_BOTTOM_TX_OUTPUT);

	if (!LED_TOP && !LED_BOTTOM)
	{
		/* Replace with blinking LEDs (timer) */
		LED_TOP = 0, LED_BOTTOM = 0;
	}
	else if (!LED_TOP && LED_BOTTOM)
	{
		LED_TOP = 1, LED_BOTTOM = 0;
	}
	else if (LED_TOP && LED_BOTTOM)
	{
		LED_TOP = 1, LED_BOTTOM = 1;
	}

	gpio_set_level(LED_WATER_LEVEL_TOP_D8_OUTPUT, LED_TOP);
	gpio_set_level(LED_WATER_LEVEL_BOTTOM_TX_OUTPUT, LED_BOTTOM);
}

uint8_t get_water_level()
{
	return NULL;
}
void water_plant(uint32_t ms)
{

}

//void setMoistureLevel(int *level)
//{
//	uint32_t color = 0x00FF0000;
//	ws2812_set_many(LED_MOISTURE_2_D3_OUTPUT, &color, (size_t) level);
//}
