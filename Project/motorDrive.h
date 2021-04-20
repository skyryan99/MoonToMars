/*
 * motorDrive.h
 *
 *  Created on: March 1, 2021
 *      Author: Schuyler Ryan
 */

#ifndef MOTORDRIVE_H_
#define MOTORDRIVE_H_

#include "msp.h"

#define M1_CW_PIN P2
#define M1_CW BIT4

#define M2_CW_PIN P2
#define M2_CW BIT6

#define M1_CCW_PIN P3
#define M1_CCW BIT6

#define M2_CCW_PIN P1
#define M2_CCW BIT7

#define M1_PWM_PIN P2
#define M1_PWM BIT5

#define M2_PWM_PIN P3
#define M2_PWM BIT0

void initMotorDrives();
void setSpeed(int32_t pow);
void initInterruptA0(uint32_t startVal);
void TA0_0_IRQHandler();

#endif /* MOTORDRIVE_H_ */
