/*
 * servoMotor.h
 *
 *  Created on: May 18, 2021
 *      Author: skyry
 */

#ifndef SERVOMOTOR_H_
#define SERVOMOTOR_H_

#define SIG_PIN P4
#define SIG_BIT BIT2

void initServoMotor();
void changeDrillDir(uint8_t dir);
void initInterruptA2();
void TA2_0_IRQHandler();

#endif /* SERVOMOTOR_H_ */
