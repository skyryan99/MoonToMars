/*
 * augerRot.h
 *
 *  Created on: May 11, 2021
 *      Author: skyry
 */

#ifndef AUGERROT_H_
#define AUGERROT_H_

#include "msp.h"

#define PWM_PIN P4
#define AUG_PWM BIT2
#define CW_PIN P4
#define AUG_CW BIT0
#define CCW_PIN P6
#define AUG_CCW BIT1

void initAugerRotMotor();
void setAugerRotSpeed(int32_t pow);
void initInterruptA3(uint32_t startVal);
void TA3_0_IRQHandler();

#endif /* AUGERROT_H_ */
