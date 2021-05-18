/*
 * augerRot.h
 *
 *  Created on: May 11, 2021
 *      Author: skyry
 */

#ifndef AUGERROT_H_
#define AUGERROT_H_

#include "msp.h"

#define AUG_ROT_PIN P4
#define AUG_ROT BIT6

void initAugerRotMotor();
void augerRotMotorOn();
void augerRotMotorOff();

#endif /* AUGERROT_H_ */
