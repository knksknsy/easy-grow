/*
 * easy_controller.h
 *
 *  Created on: 3 Jun 2019
 *      Author: Kaan Keskinsoy
 */

#ifndef MAIN_INCLUDE_EASY_CONTROLLER_H_
#define MAIN_INCLUDE_EASY_CONTROLLER_H_

typedef enum WaterLevel
{
    EMPTY,
    GOOD,
    FULL
} WaterLevel;

typedef enum MoistureLevel
{
    OFF,
    LOW,
    MID,
    HIGH
} MoistureLevel;

typedef enum Status
{
    FAILED,
    SUCCESS
} Status;

typedef struct
{
    Status status;
    uint16_t level_value;
    uint8_t level_percentage;
    MoistureLevel level_target;
} MoistureValue;

typedef struct
{
    uint16_t min;
    uint16_t max;
} MoistureLevelRange;

MoistureValue moisture_value_new(Status status, uint16_t level_value, uint8_t level_percentage, MoistureLevel level_target);
MoistureLevelRange moisture_level_range_new(uint16_t min, uint16_t max);

void moisture_button_handler(uint32_t io_num);
static void moisture_leds_handler(MoistureLevel level);
MoistureValue get_moisture_level();
MoistureLevelRange get_moisture_level_target_range(MoistureLevel level_target);
void set_moisture_level(MoistureLevel level);
void read_moisture_level(void *arg);
static uint8_t map(uint16_t x, uint16_t in_min, uint16_t in_max, uint8_t out_min, uint8_t out_max);

void photo_diode_handler(uint32_t io_num);
uint8_t get_hours_of_sun();

static void water_low_task(void *arg);
void water_level_handler();
WaterLevel get_water_level();

void activate_pump(uint32_t ms);
void deactivate_pump();

#endif /* MAIN_INCLUDE_EASY_CONTROLLER_H_ */
