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
#include "driver/adc.h"
#include "esp_timer.h"
#include "esp_sleep.h"

#include "esp_log.h"
#include "esp_system.h"
#include "sdkconfig.h"

static const char *TAG = "CONTROLLER";
volatile int water_led_task_initialized = 0;
volatile ButtonStates lbutton_states;
volatile ButtonStates rbutton_states;
volatile MoistureValue moisture_value;

MoistureValue moisture_value_new(Status status, uint16_t level_value, uint8_t level_percentage, MoistureLevel level_target)
{
	MoistureValue mv;
	mv.status = status;
	mv.level_value = level_value;
	mv.level_percentage = level_percentage;
	mv.level_target = level_target;
	return mv;
}

MoistureLevelRange moisture_level_range_new(uint16_t min, uint16_t max)
{
	MoistureLevelRange mlr;
	mlr.min = min;
	mlr.max = max;
	return mlr;
}

void read_moisture_level(void *arg)
{
	int64_t time_since_boot = esp_timer_get_time();

	ESP_LOGI(TAG, "ADC timer called, time since boot: (%d)",
			(int32_t) time_since_boot);

	MoistureValue mv = get_moisture_level();
	if (mv.status == SUCCESS)
	{
		moisture_value = mv;
	}
}

MoistureLevelRange get_moisture_level_target_range(MoistureLevel level_target)
{
	MoistureLevelRange range;
	switch (level_target)
	{
		case HIGH:
			range = moisture_level_range_new(MOISTURE_MAX_HIGH + 100, MOISTURE_HIGH);
			break;
		case MID:
			range = moisture_level_range_new(MOISTURE_HIGH + 1, MOISTURE_MID);
			break;
		case LOW:
			range = moisture_level_range_new(MOISTURE_MID + 1, MOISTURE_MAX_LOW - 100);
			break;
		case OFF:
		default:
			range = moisture_level_range_new(0, 0);
			break;
	}
	return range;
}

/*
	Re-maps a number from one range to another.
	That is, a value of fromLow would get mapped to toLow, a value of fromHigh to toHigh, values in-between to values in-between, etc.
 */
static uint8_t map(uint16_t x, uint16_t in_min, uint16_t in_max, uint8_t out_min, uint8_t out_max)
{
	return (uint8_t) ((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}

void moisture_button_handler(uint32_t io_num)
{
	/* Set ideal moisture level via hardware buttons. Sets moisture LED outputs for the configured mositure level. */

	int LED1 = gpio_get_level(LED_MOISTURE_1_D2_OUTPUT);
	int LED2 = gpio_get_level(LED_MOISTURE_2_D3_OUTPUT);
	int LED3 = gpio_get_level(LED_MOISTURE_3_D4_OUTPUT);
	MoistureLevel level_target = OFF;

	/* Decrease moisture */
	if (io_num == LBUTTON_D1_INPUT)
	{
		lbutton_states = delay_debounce(lbutton_states, LBUTTON_D1_INPUT);
		ESP_LOGI(TAG, "l_button_states: %d", lbutton_states);

		if (lbutton_states == PRESS)
		{
			if (!LED1 && !LED2 && !LED3)
			{
				// moisture value HIGH
				LED1 = 1, LED2 = 1, LED3 = 1;
				level_target = HIGH;
			}
			else if (LED1 && LED2 && LED3)
			{
				// moisture value MID
				LED1 = 1, LED2 = 1, LED3 = 0;
				level_target = MID;
			}
			else if (LED1 && LED2 && !LED3)
			{
				// moisture value LOW
				LED1 = 1, LED2 = 0, LED3 = 0;
				level_target = LOW;
			}
			else if (LED1 && !LED2 && !LED3)
			{
				// moisture value OFF
				LED1 = 0, LED2 = 0, LED3 = 0;
				level_target = OFF;
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
				// moisture value LOW
				LED1 = 1, LED2 = 0, LED3 = 0;
				level_target = LOW;
			}
			else if (LED1 && !LED2 && !LED3)
			{
				// moisture value MID
				LED1 = 1, LED2 = 1, LED3 = 0;
				level_target = MID;
			}
			else if (LED1 && LED2 && !LED3)
			{
				// moisture value HIGH
				LED1 = 1, LED2 = 1, LED3 = 1;
				level_target = HIGH;
			}
			else if (LED1 && LED2 && LED3)
			{
				// moisture value OFF
				LED1 = 0, LED2 = 0, LED3 = 0;
				level_target = OFF;
			}
		}
	}
	if (lbutton_states == PRESS || rbutton_states == PRESS)
	{
		gpio_set_level(LED_MOISTURE_1_D2_OUTPUT, LED1);
		gpio_set_level(LED_MOISTURE_2_D3_OUTPUT, LED2);
		gpio_set_level(LED_MOISTURE_3_D4_OUTPUT, LED3);
		
		moisture_value = moisture_value_new(SUCCESS, moisture_value.level_value, moisture_value.level_percentage, level_target);
		ESP_LOGI(TAG, "moisture_value: %d, %d, %d", moisture_value.level_value, moisture_value.level_percentage, moisture_value.level_target);
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

/* Read ADC input fast and return the average moisture level */
MoistureValue get_moisture_level()
{
	uint8_t x;
	uint16_t adc_data[MOISTURE_READ_DEPTH];
	volatile uint32_t avg_moisture = 0;
	uint16_t moisture_level;
	uint8_t moisture_percentage;
	MoistureValue mv;
	mv.status = FAILED;

	if (ESP_OK == adc_read_fast(adc_data, MOISTURE_READ_DEPTH))
	{
		for (x = 0; x < MOISTURE_READ_DEPTH; x++)
		{
			// printf("adc_data: %d\r\n", adc_data[x]);
			avg_moisture += adc_data[x];
		}

		avg_moisture = avg_moisture / MOISTURE_READ_DEPTH;
		
		moisture_level = (uint16_t) avg_moisture;
		//ESP_LOGI("moisture_level: %d\r\n", moisture_level);

		moisture_percentage = map(moisture_level, MOISTURE_MAX_LOW, MOISTURE_MAX_HIGH, 0, 100);
		//ESP_LOGI("moisture_percentage: %d\r\n", moisture_percentage);

		mv = moisture_value_new(SUCCESS, moisture_level, moisture_percentage, moisture_value.level_target);
		ESP_LOGI(TAG, "moisture_value: %d, %d, %d", mv.level_value, mv.level_percentage, mv.level_target);
	}
	return mv;
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

static void water_low_task(void *arg)
{
	int counter = 0;
	while (1)
	{
		counter++;
		gpio_set_level(LED_WATER_LEVEL_TOP_D8_OUTPUT, counter % 2);
		gpio_set_level(LED_WATER_LEVEL_BOTTOM_TX_OUTPUT, counter % 2);
		ESP_LOGI(TAG, "WATER LEVEL LEDS: (%d|%d)", counter % 2, counter % 2);
		vTaskDelay(1000 / portTICK_RATE_MS);
	}
}

void water_level_handler(uint32_t io_num)
{
	int WATER_TOP = gpio_get_level(WATER_LEVEL_TOP_D6_INPUT) == 0 ? 1 : 0;
	int WATER_BOTTOM = gpio_get_level(WATER_LEVEL_BOTTOM_D7_INPUT) == 0 ? 1 : 0;

	int LED_TOP = gpio_get_level(LED_WATER_LEVEL_TOP_D8_OUTPUT);
	int LED_BOTTOM = gpio_get_level(LED_WATER_LEVEL_BOTTOM_TX_OUTPUT);

	static TaskHandle_t blink_handle;

	if (WATER_TOP && WATER_BOTTOM)
	{
		vTaskSuspend(blink_handle);

		LED_BOTTOM = 1, LED_TOP = 1;
		ESP_LOGI(TAG, "WATER LEVEL LEDS: (%d|%d)", LED_BOTTOM, LED_TOP);
		gpio_set_level(LED_WATER_LEVEL_TOP_D8_OUTPUT, LED_TOP);
		gpio_set_level(LED_WATER_LEVEL_BOTTOM_TX_OUTPUT, LED_BOTTOM);

		/* Activate water pump */
	}
	else if (!WATER_TOP && WATER_BOTTOM)
	{
		vTaskSuspend(blink_handle);

		LED_BOTTOM = 1, LED_TOP = 0;
		ESP_LOGI(TAG, "WATER LEVEL LEDS: (%d|%d)", LED_BOTTOM, LED_TOP);
		gpio_set_level(LED_WATER_LEVEL_TOP_D8_OUTPUT, LED_TOP);
		gpio_set_level(LED_WATER_LEVEL_BOTTOM_TX_OUTPUT, LED_BOTTOM);

		/* Activate water pump */
	}
	else if (!WATER_TOP && !WATER_BOTTOM)
	{
		/* Create blinking LEDs task */
		if (!water_led_task_initialized)
		{
			xTaskCreate(water_low_task, "water_low_task", 2048, NULL, 10, &blink_handle);
			water_led_task_initialized = 1;
		} else {
			vTaskResume(blink_handle);
		}

		/* Deactivate water pump */
		// LED_BOTTOM = 0, LED_TOP = 0;
		// ESP_LOGI(TAG, "WATER LEVEL LEDS: (%d|%d)", LED_BOTTOM, LED_TOP);
		// gpio_set_level(LED_WATER_LEVEL_TOP_D8_OUTPUT, LED_TOP);
		// gpio_set_level(LED_WATER_LEVEL_BOTTOM_TX_OUTPUT, LED_BOTTOM);
	}
}

WaterLevel get_water_level()
{
	/* Read water sensor inputs and return water level */
	return EMPTY;
}

/* Water the plant as long as the provided milliseconds */
void activate_pump(uint32_t ms)
{
	gpio_set_level(PUMP_D0_OUTPUT, 1);
}

void deactivate_pump()
{
	gpio_set_level(PUMP_D0_OUTPUT, 0);
}

//void setMoistureLevel(int *level)
//{
//	uint32_t color = 0x00FF0000;
//	ws2812_set_many(LED_MOISTURE_2_D3_OUTPUT, &color, (size_t) level);
//}
