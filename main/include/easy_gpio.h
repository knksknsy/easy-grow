/*
 * easy_gpio.h
 *
 *  Created on: 7 May 2019
 *      Author: m.bilge
 */

#ifndef MAIN_INCLUDE_EASY_GPIO_H_

#define MAIN_INCLUDE_EASY_GPIO_H_


#define GPIO_HIGH 1
#define GPIO_LOW 0

#define WATER_LOW 0
#define WATER_MEDIUM 1
#define WATER_HIGH 2

void init_gpio();
void gpio_task(int pin,int state);




#endif /* MAIN_INCLUDE_EASY_GPIO_H_ */

