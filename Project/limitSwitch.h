/*
 * limitSwitch.h
 *
 *  Created on: Apr 20, 2021
 *      Author: Schuyler
 */

#ifndef LIMITSWITCH_H_
#define LIMITSWITCH_H_
#include "msp.h"

#define TOP_DRILL_PIN P4
#define TOP_DRILL_BIT BIT0

//#define BOT_DRILL_PIN P5
//#define BOT_DRILL_BIT BIT4

//#define TOP_AUG_PIN P4
//#define TOP_AUG_BIT BIT7

//#define BOT_AUG_PIN P4
//#define BOT_AUG_BIT BIT5

//#define LEFT_PIN P4
//#define LEFT_BIT BIT4

//#define RIGHT_PIN P4
//#define RIGHT_BIT BIT2

void initLimitSwitches();
uint8_t getError();
void resetError();

#endif /* LIMITSWITCH_H_ */
