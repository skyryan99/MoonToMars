/*
 * HeaterController.h
 *
 *  Created on: April 27, 2021
 *      Author: Jacob Everest
 */

#ifndef HEATERCONTROLLER_H_
#define HEATERCONTROLLER_H_

#define HEATER1_PIN P4
#define HEATER1_BIT BIT7   //Cartridge Heater
#define HEATER2_PIN P9
#define HEATER2_BIT BIT1   //Tube heater

void Heater1On(void);
void Heater1Off(void);
void Heater2On(void);
void Heater2Off(void);

#endif
