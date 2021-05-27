/*
 * motorDrive.h
 *
 *  Created on: March 1, 2021
 *      Author: Schuyler Ryan
 */

#ifndef MOTORDRIVER_H_
#define MOTORDRIVER_H_

#include "msp.h"

#define M2_CW_PIN P6
#define M2_CW BIT7

#define M1_CW_PIN P2
#define M1_CW BIT6

#define M2_CCW_PIN P6
#define M2_CCW BIT6

#define M1_CCW_PIN P2
#define M1_CCW BIT7

#define M2_PWM_PIN P2
#define M2_PWM BIT3

#define M1_PWM_PIN P2
#define M1_PWM BIT4

#define DRILL_POW_PIN P2
#define DRILL_POW_BIT BIT5

void initZMotorDrives();
void setDrillSpeed(int32_t pow);
void setAugerSpeed(int32_t pow);
void initInterruptA0(uint32_t startVal);
void TA0_0_IRQHandler();
void drillOn();
void drillOff();

#endif /* MOTORDRIVER_H_ */
