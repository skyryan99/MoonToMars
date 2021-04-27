/*
 * PumpController.h
 *
 *  Created on: April 27, 2021
 *      Author: Jacob Everest
 */

#ifndef PUMPCONTROLLER_H_
#define PUMPCONTROLLER_H_

#define PUMP1_PIN P6
#define PUMP1_BIT BIT6   //Connect to IN1
#define PUMP2_PIN P6
#define PUMP2_BIT BIT7   //Connect to IN2

void PumpForward(void);
void PumpBackward(void);
void PumpOff(void);

#endif
