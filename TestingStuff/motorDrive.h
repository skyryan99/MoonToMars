/*
 * motorDrive.h
 *
 *  Created on: March 1, 2021
 *      Author: Schuyler Ryan
 */

#ifndef MOTORDRIVE_H_
#define MOTORDRIVE_H_
#include "encoder.h"

#define M1_PIN P5
#define M1_BIT (BIT2|BIT7)
#define M2_PIN P3
#define M2_BIT (BIT6|BIT0)

#define Left_PIN P4
#define Left_BIT BIT4
#define Right_PIN P4
#define Right_BIT BIT7

void setMaxPowDrive(uint32_t newPow);
void initMotorDrive();
void setSpeed(int32_t pow);
uint8_t getError();
void resetError();
void disableLeftError();
void enableError();
void disableRightError();
#endif /* MOTORDRIVE_H_ */