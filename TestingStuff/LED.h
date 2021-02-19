/*
 * LED.h
 *
 *  Created on: Feb 18, 2021
 *      Author: Schuyler
 */
#ifndef LED_H_
#define LED_H_

#include "msp.h"

#define A_PIN P1
#define A_BIT BIT5
#define B_PIN P4
#define B_BIT BIT6

void initLEDs();
void delay_ms(int ms);

#endif /* LED_H_ */
