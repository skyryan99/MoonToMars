/*
 * motor.h
 *
 *  Created on: Feb 17, 2021
 *      Author: Schuyler
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#include "msp.h"

/*For configuring directional control*/
typedef enum {
    CCW, CW
}DIR;

/*For Motor - Using pins P1.5 and P4.6*/
#define A_PIN P1
#define A_BIT BIT5
#define B_PIN P4
#define B_BIT BIT6

#define DUTY_MAX 100
/*Magic number*/
#define PERIOD 333

void initMotor();
void killMotor();
void setDutyCycle(int32_t dutyCycle);
void power();

#endif /* MOTOR_H_ */
