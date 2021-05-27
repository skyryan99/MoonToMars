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

void Valve1Open(void);
void Valve1Close(void);
void Valve2Open(void);
void Valve2Close(void);

#endif
