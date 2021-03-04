/*
 * encoder.h
 *
 *  Created on: Feb 31, 2021
 *      Author: Schuyler Ryan
 */

#ifndef ENCODER_H_
#define ENCODER_H_
#include "msp.h"

#define A_PIN P5
#define A_BIT BIT5
#define B_PIN P6
#define B_BIT BIT4

#define MA_PIN P3
#define MA_BIT BIT2
#define MB_PIN P3
#define MB_BIT BIT3
enum DIR{CLOCKWISE,NOTUSED,COUNTERCLOCKWISE};

void initMotorEncoder();
void initRotEncoder();
void setValRot(int32_t newVal);
void setValMot(int32_t newVal);
enum DIR getDirRot();
enum DIR getDirMot();
int32_t getValRot();
int32_t getValMot();
uint8_t newValRot();
uint8_t newValMot();
void setMaxRot(int32_t max);

void setMinRot(int32_t min);

#endif /* ENCODER_H_ */
