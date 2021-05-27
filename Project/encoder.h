/*
 * encoder.h
 *
 *  Created on: Feb 21, 2021
 *      Author: Schuyler Ryan
 */

#ifndef ENCODER_H_
#define ENCODER_H_
#include "msp.h"

#define DRILL_A_PIN P3
#define DRILL_A_BIT BIT7
#define DRILL_B_PIN P3
#define DRILL_B_BIT BIT5

#define AUG_A_PIN P4
#define AUG_A_BIT BIT4
#define AUG_B_PIN P4
#define AUG_B_BIT BIT2

enum DIR{CLOCKWISE,NOUPDATE,COUNTERCLOCKWISE};

void initZMotorEncoders();
void setValDrill(int32_t newVal);
enum DIR getDirDrill();
int32_t getValDrill();
uint8_t newValDrill();
void setValAug(int32_t newVal);
enum DIR getDirAug();
int32_t getValAug();
uint8_t newValAug();

#endif /* ENCODER_H_ */
