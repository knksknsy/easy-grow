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
static volatile ButtonStates lbutton_states;
static volatile ButtonStates rbutton_states;
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

	ESP_LOGI(TAG, "ADC timer called, time since boot: (%d)", (int32_t)time_since_boot);
	pump_handler(get_moisture_level());
}

MoistureLevelRange get_moisture_level_target_range(MoistureLevel level_target)
{
	MoistureLevelRange range;
	switch (level_target)
	{
	case HIGH:
		// test case: moisture level is actually in the range of 250 to 450 if the sensor is placed in water (100% moisture)
		range = moisture_level_range_new(0, MOISTURE_HIGH);
		// range = moisture_level_range_new(MOISTURE_MAX_HIGH + 100, MOISTURE_HIGH);
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
	return (uint8_t)((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}

/* Set ideal moisture level via hardware buttons. Sets moisture LED outputs for the configured mositure level. */
void moisture_button_handler(uint32_t io_num)
{
	int LED1 = gpio_get_level(LED_MOISTURE_1_D2_OUTPUT);
	int LED2 = gpio_get_level(LED_MOISTURE_2_D3_OUTPUT);
	int LED3 = gpio_get_level(LED_MOISTURE_3_D4_OUTPUT);

	MoistureLevel level_target = OFF;

	/* Decrease moisture level */
	if (io_num == LBUTTON_D1_INPUT)
	{
		lbutton_states = delay_debounce(lbutton_states, LBUTTON_D1_INPUT);

		if (lbutton_states == PRESS)
		{
			ESP_LOGI(TAG, "l_button_states: %d", lbutton_states);
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
			gpio_set_level(LED_MOISTURE_1_D2_OUTPUT, LED1);
			gpio_set_level(LED_MOISTURE_2_D3_OUTPUT, LED2);
			gpio_set_level(LED_MOISTURE_3_D4_OUTPUT, LED3);

			moisture_value = moisture_value_new(SUCCESS, moisture_value.level_value, moisture_value.level_percentage, level_target);
			ESP_LOGI(TAG, "moisture_value: %d, %d, %d", moisture_value.level_value, moisture_value.level_percentage, moisture_value.level_target);
		}
	}
	/* Increase moisture level */
	else if (io_num == RBUTTON_D5_INPUT)
	{
		rbutton_states = delay_debounce(rbutton_states, RBUTTON_D5_INPUT);

		if (rbutton_states == PRESS)
		{
			ESP_LOGI(TAG, "r_button_states: %d", rbutton_states);
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
			gpio_set_level(LED_MOISTURE_1_D2_OUTPUT, LED1);
			gpio_set_level(LED_MOISTURE_2_D3_OUTPUT, LED2);
			gpio_set_level(LED_MOISTURE_3_D4_OUTPUT, LED3);

			moisture_value = moisture_value_new(SUCCESS, moisture_value.level_value, moisture_value.level_percentage, level_target);
			ESP_LOGI(TAG, "moisture_value: %d, %d, %d", moisture_value.level_value, moisture_value.level_percentage, moisture_value.level_target);
		}
	}
}

/* Set moisture LED outputs for the configured moisture level (server-side configuration of moisture level). */
static void moisture_leds_handler(MoistureLevel level)
{
	int LED1 = 0;
	int LED2 = 0;
	int LED3 = 0;
	switch (level)
	{
	case OFF:
		LED1 = 0, LED2 = 0, LED3 = 0;
		break;
	case LOW:
		LED1 = 1, LED2 = 0, LED3 = 0;
		break;
	case MID:
		LED1 = 1, LED2 = 1, LED3 = 0;
		break;
	case HIGH:
		LED1 = 1, LED2 = 1, LED3 = 1;
		break;
	default:
		break;
	}

	gpio_set_level(LED_MOISTURE_1_D2_OUTPUT, LED1);
	gpio_set_level(LED_MOISTURE_2_D3_OUTPUT, LED2);
	gpio_set_level(LED_MOISTURE_3_D4_OUTPUT, LED3);
}

/* Set the ideal value for moisture (server-side configuration of moisture level). */
void set_moisture_level(MoistureLevel level)
{
	moisture_leds_handler(level);
	MoistureValue mv = get_moisture_level();
	if (mv.status == SUCCESS)
	{
		moisture_value = moisture_value_new(SUCCESS, mv.level_value, mv.level_percentage, level);
		ESP_LOGI(TAG, "moisture_value: %d, %d, %d", moisture_value.level_value, moisture_value.level_percentage, moisture_value.level_target);
	}
	else
	{
		moisture_value = moisture_value_new(SUCCESS, moisture_value.level_value, moisture_value.level_percentage, level);
		ESP_LOGI(TAG, "moisture_value: %d, %d, %d", moisture_value.level_value, moisture_value.level_percentage, moisture_value.level_target);
	}
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

		moisture_level = (uint16_t)avg_moisture;
		//ESP_LOGI("moisture_level: %d\r\n", moisture_level);

		moisture_percentage = map(moisture_level, MOISTURE_MAX_LOW, MOISTURE_MAX_HIGH, 0, 100);
		//ESP_LOGI("moisture_percentage: %d\r\n", moisture_percentage);

		mv = moisture_value_new(SUCCESS, moisture_level, moisture_percentage, moisture_value.level_target);
		ESP_LOGI(TAG, "moisture_value: %d, %d, %d", mv.level_value, mv.level_percentage, mv.level_target);
	}
	return mv;
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

void water_level_handler()
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
	}
	else if (!WATER_TOP && WATER_BOTTOM)
	{
		vTaskSuspend(blink_handle);

		LED_BOTTOM = 1, LED_TOP = 0;
		ESP_LOGI(TAG, "WATER LEVEL LEDS: (%d|%d)", LED_BOTTOM, LED_TOP);
		gpio_set_level(LED_WATER_LEVEL_TOP_D8_OUTPUT, LED_TOP);
		gpio_set_level(LED_WATER_LEVEL_BOTTOM_TX_OUTPUT, LED_BOTTOM);
	}
	else if (!WATER_TOP && !WATER_BOTTOM)
	{
		/* Create blinking LEDs task */
		if (!water_led_task_initialized)
		{
			xTaskCreate(water_low_task, "water_low_task", 2048, NULL, 10, &blink_handle);
			water_led_task_initialized = 1;
		}
		else
		{
			vTaskResume(blink_handle);
		}
		ESP_LOGI(TAG, "deactivate_pump() inside of water_level_handler EMPTY WATER LEVEL");
		deactivate_pump();
	}
}

/* Read water sensor inputs and return water level */
WaterLevel get_water_level()
{
	int WATER_TOP = gpio_get_level(WATER_LEVEL_TOP_D6_INPUT) == 0 ? 1 : 0;
	int WATER_BOTTOM = gpio_get_level(WATER_LEVEL_BOTTOM_D7_INPUT) == 0 ? 1 : 0;

	WaterLevel water_level = EMPTY;

	if (WATER_TOP && WATER_BOTTOM)
	{
		water_level = FULL;
	}
	else if (!WATER_TOP && WATER_BOTTOM)
	{
		water_level = GOOD;
	}
	else if (!WATER_TOP && !WATER_BOTTOM)
	{
		water_level = EMPTY;
	}
	return water_level;
}

static void pump_handler(MoistureValue mv)
{
	if (mv.status == SUCCESS)
	{
		moisture_value = mv;
		MoistureLevelRange moisture_level_range = get_moisture_level_target_range(moisture_value.level_target);

		uint8_t moisture_in_range = moisture_value.level_value >= moisture_level_range.min && moisture_value.level_value <= moisture_level_range.max;
		uint8_t moisture_out_of_min_range = moisture_value.level_value < moisture_level_range.min;
		uint8_t moisture_out_of_max_range = moisture_value.level_value > moisture_level_range.max;
		uint8_t moisture_monitoring_off = moisture_value.level_target == OFF;

		if (moisture_in_range || moisture_out_of_min_range || moisture_monitoring_off)
		{
			ESP_LOGI(TAG, "deactivate_pump() inside of pump_handler IN_RANGE || OUT_MIN_RANGE || MONITORING OFF");
			deactivate_pump();
		}
		else if (moisture_out_of_max_range)
		{
			WaterLevel water_level = get_water_level();
			switch (water_level)
			{
			case EMPTY:
				ESP_LOGI(TAG, "deactivate_pump() inside of pump_handler EMPTY WATER LEVEL");
				deactivate_pump();
				break;
			case GOOD:
			case FULL:
				activate_pump(PUMP_INTERVAL);
				break;
			default:
				break;
			}
		}
	}
}

/* Water the plant as long as the provided milliseconds. Use ms = 0 for default interval of 5 seconds */
void activate_pump(uint32_t ms)
{
	if (ms == 0)
	{
		ms = PUMP_INTERVAL;
	}
	gpio_set_level(PUMP_D0_OUTPUT, 1);
	ESP_LOGI(TAG, "activate_pump(%d)", ms);
	vTaskDelay(ms / portTICK_PERIOD_MS);
	//water_level_handler();
	ESP_LOGI(TAG, "deactivate_pump() inside of activate_pump");
	deactivate_pump();
}

void deactivate_pump()
{
	gpio_set_level(PUMP_D0_OUTPUT, 0);
	ESP_LOGI(TAG, "deactivate_pump()");
}

/* Read photo diode input and count the hours of sun for the current day */
void photo_diode_handler(uint32_t io_num)
{
	int value = gpio_get_level(io_num);
	ESP_LOGI(TAG, "photo diode: %d", value);
}

/* Return hours of sun registered for the current day */
uint8_t get_hours_of_sun()
{
	return NULL;
}
