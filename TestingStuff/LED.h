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

#define DELAY_FOR_1_MHZ 1000
#define FREQ_1_5MHZ 1
#define FREQ_3MHZ 3
#define FREQ_6MHZ 6
#define FREQ_12MHZ 12
#define FREQ_24MHZ 24
#define FREQ_48MHZ 48

void initLEDs();
void delay_ms(uint32_t ms, uint32_t freq);
void set_DCO(uint32_t freq);

#endif /* LED_H_ */
