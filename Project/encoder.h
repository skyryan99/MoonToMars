/*
 * encoder.h
 *
 *  Created on: Feb 21, 2021
 *      Author: Schuyler Ryan
 */

#ifndef ENCODER_H_
#define ENCODER_H_
#include "msp.h"

#define MA_PIN P3
#define MA_BIT BIT2
#define MB_PIN P3
#define MB_BIT BIT3
enum DIR{CLOCKWISE,NOTUSED,COUNTERCLOCKWISE};

void initMotorEncoders();
void setValMot(int32_t newVal);
enum DIR getDirMot();
int32_t getValMot();
uint8_t newValMot();
void setMaxRot(int32_t max);

#endif /* ENCODER_H_ */
