/*
 * easy_controller.c
 * 
 * This file is the controller of the easy grow watering system.
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

// Button states for detecting debounced button pushes
static volatile ButtonStates lbutton_states;
static volatile ButtonStates rbutton_states;

// Global MoistureValue struct containing the most recent moisture values and settings
volatile MoistureValue moisture_value;

/**
 * Constructor for creating a new MoistureValue struct.
 * 
 * Status status:				Status whether soil moisture level reading was successful or not.
 * uint16_t level_value:		Soil moisture level (range: 0 to 1023)
 * uint8_t level_percentage:	Mapped soil moisture level in percent (0% to 100%)
 * MoisuteLevel level_taget:	Desired soil moisture level which sustains a specific 'level_value' (4 settings: OFF, LOW, MID, HIGH)
 * Returns a MoistureValue struct.
 */
MoistureValue moisture_value_new(Status status, uint16_t level_value, uint8_t level_percentage, MoistureLevel level_target)
{
	MoistureValue mv;
	mv.status = status;
	mv.level_value = level_value;
	mv.level_percentage = level_percentage;
	mv.level_target = level_target;
	return mv;
}

/**
 * Constructor for creating a new MoistureLevelRange struct.
 * This struct holds the level ranges (min and max) of a specified moisture setting (OFF, LOW, MID, HIGH)
 * 
 * uint16_t min: Min value of specified moisture setting
 * uint16_t max: Max value of specified moisture setting
 * Returns a MoistureLevelRange struct
 */
MoistureLevelRange moisture_level_range_new(uint16_t min, uint16_t max)
{
	MoistureLevelRange mlr;
	mlr.min = min;
	mlr.max = max;
	return mlr;
}

/**
 * Read soil moisture level.
 */
void read_moisture_level(void *arg)
{
	int64_t time_since_boot = esp_timer_get_time();

	ESP_LOGI(TAG, "ADC timer called, time since boot: (%d)", (int32_t)time_since_boot);
	// Get moisture level and pass it to the pump handler
	pump_handler(get_moisture_level());
}

/**
 * Get a MoistureLevelRange struct from the provided MoistureLevel enum.
 * 
 * MoistureLevel level_target: enum for setting the desired moisture level (OFF, LOW, MID, HIGH)
 * Return MoistureLevelRange 
 */
MoistureLevelRange get_moisture_level_target_range(MoistureLevel level_target)
{
	MoistureLevelRange range;
	switch (level_target)
	{
	case HIGH:
		range = moisture_level_range_new(MOISTURE_MAX_HIGH + 100, MOISTURE_HIGH);
		// test case: moisture level is actually in the range of 250 to 450 if the sensor is placed in water (100% moisture)
		// range = moisture_level_range_new(0, MOISTURE_HIGH);
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

/**
 * Re-maps a number from one range to another.
 * That is, a value of fromLow would get mapped to toLow, a value of fromHigh to toHigh, values in-between to values in-between, etc.
 * 
 * uit8_t x:		Value to be mapped to a specific value
 * uint16_t in_min:	x value's min value
 * uint16_t in_max:	x value's max value
 * uint8_t out_min: Desired min value of the x value. The x value will be mapped in-between the out_min and out_max values.
 * uint8_t out_max: Desired max value of the x value. The x value will be mapped in-between the out_min and out_max values.
 * Returns uint8_t of the mapped value.
 */
static uint8_t map(uint16_t x, uint16_t in_min, uint16_t in_max, uint8_t out_min, uint8_t out_max)
{
	return (uint8_t)((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}

/**
 * Set ideal moisture level via hardware buttons. Sets moisture LED outputs for the configured mositure level.
 * 
 * uint32_t io_num: Moisture button GPIOs' pin number (up/down)
 */
void moisture_button_handler(uint32_t io_num)
{
	// Get the current state of the LEDs
	int LED1 = gpio_get_level(LED_MOISTURE_1_D2_OUTPUT);
	int LED2 = gpio_get_level(LED_MOISTURE_2_D3_OUTPUT);
	int LED3 = gpio_get_level(LED_MOISTURE_3_D4_OUTPUT);

	MoistureLevel level_target = OFF;

	// Decrease moisture level
	if (io_num == LBUTTON_D1_INPUT)
	{
		// Get pressbutton's debounced value
		lbutton_states = delay_debounce(lbutton_states, LBUTTON_D1_INPUT);

		if (lbutton_states == PRESS)
		{
			ESP_LOGI(TAG, "l_button_states: %d", lbutton_states);
			// Cycle and decrement LEDs in reverse order
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

			// Set the state of the LEDs
			gpio_set_level(LED_MOISTURE_1_D2_OUTPUT, LED1);
			gpio_set_level(LED_MOISTURE_2_D3_OUTPUT, LED2);
			gpio_set_level(LED_MOISTURE_3_D4_OUTPUT, LED3);

			// Save the set level_target (OFF, LOW, MID, HIGH)
			moisture_value = moisture_value_new(SUCCESS, moisture_value.level_value, moisture_value.level_percentage, level_target);
			ESP_LOGI(TAG, "moisture_value: %d, %d, %d", moisture_value.level_value, moisture_value.level_percentage, moisture_value.level_target);
		}
	}
	// Increase moisture level
	else if (io_num == RBUTTON_D5_INPUT)
	{
		// Get pressbutton's debounced value
		rbutton_states = delay_debounce(rbutton_states, RBUTTON_D5_INPUT);

		if (rbutton_states == PRESS)
		{
			ESP_LOGI(TAG, "r_button_states: %d", rbutton_states);
			// Cycle and increment LEDs
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

			// Set the state of the LEDs
			gpio_set_level(LED_MOISTURE_1_D2_OUTPUT, LED1);
			gpio_set_level(LED_MOISTURE_2_D3_OUTPUT, LED2);
			gpio_set_level(LED_MOISTURE_3_D4_OUTPUT, LED3);

			// Save the set level_target (OFF, LOW, MID, HIGH)
			moisture_value = moisture_value_new(SUCCESS, moisture_value.level_value, moisture_value.level_percentage, level_target);
			ESP_LOGI(TAG, "moisture_value: %d, %d, %d", moisture_value.level_value, moisture_value.level_percentage, moisture_value.level_target);
		}
	}
}

/**
 * Set moisture LED outputs for the configured moisture level (server-side configuration of moisture level).
 * 
 * MoistureLevel level: enum of the desired moisture level setting (OFF, LOW, MID, HIGH)
 */
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

/**
 * Set the ideal value for moisture (server-side configuration of moisture level).
 * 
 * MoistureLevel level: enum of the desired moisture level setting (OFF, LOW, MID, HIGH)
 */
void set_moisture_level(MoistureLevel level)
{
	ESP_LOGI(TAG, "Changing Moisture level: %d", level);
	// Set the state of the moisture LEDs
	moisture_leds_handler(level);

	MoistureValue mv = get_moisture_level();
	if (mv.status == SUCCESS)
	{
		// Save the target moisture level setting
		moisture_value = moisture_value_new(SUCCESS, mv.level_value, mv.level_percentage, level);
		ESP_LOGI(TAG, "moisture_value: %d, %d, %d", moisture_value.level_value, moisture_value.level_percentage, moisture_value.level_target);
	}
	else
	{
		// Save the target moisture level setting
		moisture_value = moisture_value_new(SUCCESS, moisture_value.level_value, moisture_value.level_percentage, level);
		ESP_LOGI(TAG, "moisture_value: %d, %d, %d", moisture_value.level_value, moisture_value.level_percentage, moisture_value.level_target);
	}
}

/**
 * Read ADC input fast and return the average moisture level.
 * 
 * Returns a MoistureValue struct holding the most recent moisture values and settings
 */
MoistureValue get_moisture_level()
{
	uint8_t x;
	uint16_t adc_data[MOISTURE_READ_DEPTH];
	volatile uint32_t avg_moisture = 0;
	uint16_t moisture_level;
	uint8_t moisture_percentage;
	MoistureValue mv;
	// Status signaling if reading the moisture level has failed
	mv.status = FAILED;

	if (ESP_OK == adc_read_fast(adc_data, MOISTURE_READ_DEPTH))
	{
		// Calculate the average soil moisture level
		for (x = 0; x < MOISTURE_READ_DEPTH; x++)
		{
			// printf("adc_data: %d\r\n", adc_data[x]);
			avg_moisture += adc_data[x];
		}
		avg_moisture = avg_moisture / MOISTURE_READ_DEPTH;

		// Typecasting the soil moisture level
		moisture_level = (uint16_t)avg_moisture;
		//ESP_LOGI("moisture_level: %d\r\n", moisture_level);

		// Mapping of the soil moisture level to a more humanly readable value (0% to 100%)
		moisture_percentage = map(moisture_level, MOISTURE_MAX_LOW, MOISTURE_MAX_HIGH, 0, 100);
		//ESP_LOGI("moisture_percentage: %d\r\n", moisture_percentage);

		// Initializing of a new MoistureValue struct containing the most recent values and settings
		mv = moisture_value_new(SUCCESS, moisture_level, moisture_percentage, moisture_value.level_target);
		ESP_LOGI(TAG, "moisture_value: %d, %d, %d", mv.level_value, mv.level_percentage, mv.level_target);
	}
	return mv;
}

/**
 * Read water sensor GPIOs and return WaterLevel enum.
 * 
 * Returns WaterLevel enum showing the current water level
 */
WaterLevel get_water_level()
{
	uint8_t WATER_TOP = gpio_get_level(WATER_LEVEL_TOP_D6_INPUT) == 0 ? 1 : 0;
	uint8_t WATER_BOTTOM = gpio_get_level(WATER_LEVEL_BOTTOM_D7_INPUT) == 0 ? 1 : 0;

	WaterLevel water_level = EMPTY;

	if (WATER_TOP && WATER_BOTTOM)
	{
		water_level = FULL;
		// Set the LED state to FULL
		water_level_leds_handler(1, 1);
	}
	else if (!WATER_TOP && WATER_BOTTOM)
	{
		water_level = GOOD;
		// Set the LED state to GOOD
		water_level_leds_handler(1, 0);
	}
	else if (!WATER_TOP && !WATER_BOTTOM)
	{
		water_level = EMPTY;
		// Set the LED state to EMPTY
		water_level_leds_handler(0, 0);
	}
	return water_level;
}

/**
 * Handler for the water level LEDs.
 * 
 * uint8_t LED_BOTTOM:	Either 0 or 1 whether the bottom LED should be on or off.
 * uint8_t LED_TOP:		Either 0 or 1 whether the top LED should be on or off.
 */
void water_level_leds_handler(uint8_t LED_BOTTOM, uint8_t LED_TOP)
{
	gpio_set_level(LED_WATER_LEVEL_BOTTOM_TX_OUTPUT, LED_BOTTOM);
	gpio_set_level(LED_WATER_LEVEL_TOP_D8_OUTPUT, LED_TOP);
	ESP_LOGI(TAG, "WATER LEVEL LEDS: (%d|%d)", LED_BOTTOM, LED_TOP);
}

/**
 * Water pump handler. If moisture level is not in the desired range the system will water the plant.
 * 
 * MoistureValue mv: MoistureValue struct containing the most recent moisture values and settings
 */
static void pump_handler(MoistureValue mv)
{
	if (mv.status == SUCCESS)
	{
		moisture_value = mv;
		// Get the min max range of the moisture level for a specified moisture setting (level_target: OFF, LOW, MID, HIGH)
		MoistureLevelRange moisture_level_range = get_moisture_level_target_range(moisture_value.level_target);

		uint8_t moisture_monitoring_off = moisture_value.level_target == OFF;
		uint8_t moisture_in_range = moisture_value.level_value >= moisture_level_range.min && moisture_value.level_value <= moisture_level_range.max;
		uint8_t moisture_out_of_min_range = moisture_value.level_value < moisture_level_range.min;
		uint8_t moisture_out_of_max_range = moisture_value.level_value > moisture_level_range.max;

		// Deactivate pump if moisture level is in range, is over watered or the monitoring is off
		if (moisture_in_range || moisture_out_of_min_range || moisture_monitoring_off)
		{
			deactivate_pump();
		}
		// If plant is under watered continue pump handling routine
		else if (moisture_out_of_max_range)
		{
			// Read the water level in the water tank
			WaterLevel water_level = get_water_level();
			switch (water_level)
			{
			// Deactivate pump if water tank is empty
			case EMPTY:
				deactivate_pump();
				break;
			// Activate pump if water tank is not empty
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

/**
 * Water the plant as long as the provided milliseconds. Use ms = 0 for default interval of PUMP_INTERVAL milliseconds.
 * 
 * uint32_t ms: Duration of the activation of the water pump in milliseconds
 */
void activate_pump(uint32_t ms)
{
	if (ms == 0)
	{
		ms = PUMP_INTERVAL;
	}
	gpio_set_level(PUMP_D0_OUTPUT, 1);
	ESP_LOGI(TAG, "activate_pump(%d)", ms);
	vTaskDelay(ms / portTICK_RATE_MS);
	//water_level_handler();
	ESP_LOGI(TAG, "deactivate_pump()");
	deactivate_pump();
}

/**
 * Deactivating the water pump.
 */
void deactivate_pump()
{
	gpio_set_level(PUMP_D0_OUTPUT, 0);
	ESP_LOGI(TAG, "deactivate_pump()");
}

/**
 * Initialize sun hours counter.
 */
void init_sun_hours_counter()
{
	// State of the photo diode indicates whether to start counting 'day time' or 'night time'.
	// 'day time'	state:		PREV_STATE = 1 && STATE = 0		('light on' to 'light off')
	// 'night time'	state:		PREV_STATE = 0 && STATE = 1		('light off' to 'light on')
	const uint8_t PREV_STATE = gpio_get_level(PHOTO_DIODE_RX_INPUT);
	saveFlash(PHOTO_DIODE_PREV_STATE_ADR, PREV_STATE);

	// Save previous time to calulate delta t
	// delta t = 'current time' - PREV_TIME
	const uint32_t PREV_TIME = (uint32_t)esp_timer_get_time();
	saveFlash(PHOTO_DIODE_PREV_TIME_ADR, PREV_TIME);

	// Initialize 'day time' counter 0
	saveFlash(PHOTO_DIODE_TIME_DAY, 0);
	// Initialize 'night time' counter 0
	saveFlash(PHOTO_DIODE_TIME_NIGHT, 0);
}

/**
 * Read photo diode input and count the hours of sun for the current day.
 * 
 * uint32_t io_num: Photo diode's GPIO pin number
 */
void photo_diode_handler(uint32_t io_num)
{
	// Current state of photo diode ('light on' or 'light off')
	const uint8_t STATE = gpio_get_level(io_num);
	// Read previous state of photo diode
	const uint8_t PREV_STATE = readFlash(PHOTO_DIODE_PREV_STATE_ADR);

	// Check transition from 'light on' to 'light off' (vice versa)
	if (STATE != PREV_STATE)
	{
		// Indicators for wheter counting 'day time' or 'night time'
		const uint8_t TIME_RANGE_DAY = STATE && !PREV_STATE;
		const uint8_t TIME_RANGE_NIGHT = !STATE && PREV_STATE;

		// Read current time from esp_timer
		const uint32_t TIME = (uint32_t)esp_timer_get_time();
		// Read previous time
		const uint32_t PREV_TIME = readFlash(PHOTO_DIODE_PREV_TIME_ADR);

		// Save current STATE and current Time for calculating 'delta time' in next ISR
		saveFlash(PHOTO_DIODE_PREV_STATE_ADR, STATE);
		saveFlash(PHOTO_DIODE_PREV_TIME_ADR, TIME);

		// Calculate 'delta t' and convert from micro seconds to seconds
		const uint32_t TIME_DELTA = (TIME - PREV_TIME) / 1000000;

		// Read total 'day time' and 'night time' values from memory
		uint32_t TOTAL_TIME_DAY = readFlash(PHOTO_DIODE_TIME_DAY);
		uint32_t TOTAL_TIME_NIGHT = readFlash(PHOTO_DIODE_TIME_NIGHT);

		// Reset total times (day and night) after 24 h
		if (TOTAL_TIME_DAY + TOTAL_TIME_NIGHT >= 86400)
		{
			saveFlash(PHOTO_DIODE_TIME_DAY, 0);
			saveFlash(PHOTO_DIODE_TIME_NIGHT, 0);
		}
		// Continue counting 'day time' and 'night time'
		else
		{
			// Calculate new total 'day time'
			if (TIME_RANGE_DAY)
			{
				TOTAL_TIME_DAY += TIME_DELTA;
				saveFlash(PHOTO_DIODE_TIME_DAY, TOTAL_TIME_DAY);
			}
			// Calculate new total 'night time'
			else if (TIME_RANGE_NIGHT)
			{
				TOTAL_TIME_NIGHT += TIME_DELTA;
				saveFlash(PHOTO_DIODE_TIME_NIGHT, TOTAL_TIME_NIGHT);
			}
		}
	}
}

/**
 * Return hours of sun registered for the current day.
 */
uint8_t get_hours_of_sun()
{
	return (readFlash(PHOTO_DIODE_TIME_DAY) / 3600);
}
