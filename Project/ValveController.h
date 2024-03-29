/*
 * ValveController.h
 *
 *  Created on: April 22, 2021
 *      Author: Jacob Everest
 */

#ifndef VALVECONTROLLER_H_
#define VALVECONTROLLER_H_

#define VALVE1_PIN P6
#define VALVE1_BIT BIT3
#define VALVE2_PIN P9
#define VALVE2_BIT BIT3
#define VALVE3_PIN P8
#define VALVE3_BIT BIT3

#define DELAY 1500//how many ms to wait for valve to actuate

void initValves();
void Valve1Open(void);
void Valve1Close(void);
void Valve2Open(void);
void Valve2Close(void);
void Valve3Open(void);
void Valve3Close(void);

#endif
