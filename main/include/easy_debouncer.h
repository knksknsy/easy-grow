/*
 * debouncer.h
 *
 *  Created on: 3 Jun 2019
 *      Author: Kaan Keskinsoy
 */

#ifndef MAIN_INCLUDE_EASY_DEBOUNCER_H_
#define MAIN_INCLUDE_EASY_DEBOUNCER_H_

// Enum for button push states.
typedef enum {
    UP,
    DOWN,
    PRESS,
    RELEASE
} ButtonStates;

ButtonStates delay_debounce(ButtonStates button_state, int gpio_num);

#endif /* MAIN_INCLUDE_EASY_DEBOUNCER_H_ */
