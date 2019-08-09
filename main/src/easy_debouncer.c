/*
 * debouncer.c
 * 
 * This file is responsible for the detection of debounced button pushes.
 *
 *  Created on: 3 Jun 2019
 *      Author: Kaan Keskinsoy
 */

#include "easy_debouncer.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"

static const char *TAG = "DEBOUNCER";

/**
 * Recognized debounced button pushes.
 * 
 * ButtonStates button_state:	enum holding the information of a button activation
 * int gpio_num:				GPIO pin number of the pushbutton
 * 
 * Returns ButtonStates enum showing whether a button is pushed or not.
 */
ButtonStates delay_debounce(ButtonStates button_state, int gpio_num)
{
	// if pressed
	if (gpio_get_level(gpio_num))
	{
		if (button_state == PRESS)
		{
			button_state = DOWN;
		}
		if (button_state == UP)
		{
			vTaskDelay(25 / portTICK_RATE_MS);
			if (gpio_get_level(gpio_num) == 1)
			{
				button_state = PRESS;
			}
		}
	}
	// if not pressed
	else
	{
		if (button_state == RELEASE)
		{
			button_state = UP;
		}
		if (button_state == DOWN)
		{
			if (gpio_get_level(gpio_num) == 0)
			{
				vTaskDelay(25 / portTICK_RATE_MS);
				if (gpio_get_level(gpio_num) == 0)
				{
					button_state = RELEASE;
				}
			}
		}
	}
	return button_state;
}
