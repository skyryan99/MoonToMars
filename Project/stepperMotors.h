/*
 * stepperMotor.h
 *
 *  Created on: April 4, 2021
 *      Author: Jacob Everest
 */

#ifndef STEPPERMOTOR_H_
#define STEPPERMOTOR_H_

#define DIR_PIN P1
#define DIR_BIT BIT6
#define PUL_PIN P2
#define PUL_BIT BIT3
#define ENA_PIN P5
#define ENA_BIT BIT0

void initStepperMotor();
void MoveLeft(uint32_t runtime);
void MoveRight(uint32_t runtime);

#endif
