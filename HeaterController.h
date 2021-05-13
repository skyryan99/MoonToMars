/*
 * PumpController.h
 *
 *  Created on: April 27, 2021
 *      Author: Jacob Everest
 */
#ifndef HEATERCONTROLLER_H_
#define HEATERCONTROLLER_H_

#define HEATER1_PIN P6
#define HEATER1_BIT BIT5   //Connect to IN1
#define HEATER2_PIN P4
#define HEATER2_BIT BIT6   //Connect to IN2

void Heater1On(void);
void Heater1Off(void);
void Heater2On(void);
void Heater2Off(void);

#endif
