/*
 * limitSwitch.h
 *
 *  Created on: Apr 20, 2021
 *      Author: Schuyler
 */

#ifndef LIMITSWITCH_H_
#define LIMITSWITCH_H_
#include "msp.h"

#define LIM_PIN P6
#define LIM_BIT BIT0

#define ADCMAX 0x3FFF

void initLimitSwitches();
uint8_t getError();
void resetError();
float getVal();
void readADC();

#endif /* LIMITSWITCH_H_ */
