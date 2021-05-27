/*
 * servoMotor.h
 *
 *  Created on: May 18, 2021
 *      Author: skyry
 */

#ifndef SERVOMOTOR_H_
#define SERVOMOTOR_H_

#define SIG_PIN P10
#define SIG_BIT BIT5

void initServoMotor();
void reverseServo(uint32_t clock_freq);
void forwardServo(uint32_t clock_freq);

#endif /* SERVOMOTOR_H_ */
