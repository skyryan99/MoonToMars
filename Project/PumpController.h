/*
 * PumpController.h
 *
 *  Created on: April 27, 2021
 *      Author: Jacob Everest
 */
#ifndef PUMPCONTROLLER_H_
#define PUMPCONTROLLER_H_

#define PUMP1_PIN P4
#define PUMP1_BIT BIT3   //Connect to IN1
#define PUMP2_PIN P4
#define PUMP2_BIT BIT1   //Connect to IN2
#define PUMPPWM_PIN P4
#define PUMPPWM_BIT BIT6

void PumpForward(void);
void PumpBackward(void);
void PumpOff(void);

#endif
