/*
 * easy_controller.h
 *
 *  Created on: 3 Jun 2019
 *      Author: Kaan Keskinsoy
 */

#ifndef MAIN_INCLUDE_EASY_CONTROLLER_H_
#define MAIN_INCLUDE_EASY_CONTROLLER_H_

#define PHOTO_DIODE_PREV_STATE_ADR      0
#define PHOTO_DIODE_PREV_TIME_ADR       1
#define PHOTO_DIODE_TIME_DAY            2
#define PHOTO_DIODE_TIME_NIGHT          3

// Enum signaling the level of the water tank
typedef enum WaterLevel
{
    EMPTY,
    GOOD,
    FULL
} WaterLevel;

// Enum signaling the target soil moisture level setting
typedef enum MoistureLevel
{
    OFF,
    LOW,
    MID,
    HIGH
} MoistureLevel;

// Enum signaling if the soil moisture level was read successfully or not
typedef enum Status
{
    FAILED,
    SUCCESS
} Status;

// Struct for soil moisture level information and the target MoistureLevel
typedef struct
{
    Status status;
    uint16_t level_value;
    uint8_t level_percentage;
    MoistureLevel level_target;
} MoistureValue;

// Struct holding the min and max value of a MoistureLevel
typedef struct
{
    uint16_t min;
    uint16_t max;
} MoistureLevelRange;

MoistureValue moisture_value_new(Status status, uint16_t level_value, uint8_t level_percentage, MoistureLevel level_target);
MoistureLevelRange moisture_level_range_new(uint16_t min, uint16_t max);

static void pump_handler(MoistureValue mv);
void moisture_button_handler(uint32_t io_num);
static void moisture_leds_handler(MoistureLevel level);
MoistureValue get_moisture_level();
MoistureLevelRange get_moisture_level_target_range(MoistureLevel level_target);
void set_moisture_level(MoistureLevel level);
void read_moisture_level(void *arg);
static uint8_t map(uint16_t x, uint16_t in_min, uint16_t in_max, uint8_t out_min, uint8_t out_max);

void init_sun_hours_counter();
void photo_diode_handler(uint32_t io_num);
uint8_t get_hours_of_sun();

WaterLevel get_water_level();
void water_level_leds_handler(uint8_t LED_BOTTOM, uint8_t LED_TOP);

void activate_pump(uint32_t ms);
void deactivate_pump();

#endif /* MAIN_INCLUDE_EASY_CONTROLLER_H_ */
