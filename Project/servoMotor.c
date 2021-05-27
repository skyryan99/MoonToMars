/*
 * servoMotor.c
 *
 *  Created on: May 18, 2021
 *      Author: skyry
 */

#include "msp.h"
#include "servoMotor.h"
#include "delay.h"

void initServoMotor()//can rework to run on interrupts, but probs not necessary. nothing should run during this time?
{
    SIG_PIN->SEL0 &= ~SIG_BIT;
    SIG_PIN->SEL1 &= ~SIG_BIT;
    SIG_PIN->DIR  |= SIG_BIT;
    SIG_PIN->OUT  &= ~SIG_BIT;
}

void forwardServo(uint32_t clock_freq)//blocks for 20ms
{
    SIG_PIN->OUT ^= SIG_BIT;
    delay_ms(2, clock_freq);
    SIG_PIN->OUT ^= SIG_BIT;
    delay_ms(18, clock_freq);
}

void reverseServo(uint32_t clock_freq)//blocks for 20ms
{
    SIG_PIN->OUT ^= SIG_BIT;
    delay_ms(1, clock_freq);
    SIG_PIN->OUT ^= SIG_BIT;
    delay_ms(19, clock_freq);
}
