/*
 * servoMotor.c
 *
 *  Created on: May 18, 2021
 *      Author: skyry
 */

#include "msp.h"
#include "servoMotor.h"
#include "motorDriver.h"

#define PERIOD 24000

static uint32_t time;//time for servo motor to run for

void initServoMotor()//can rework to run on interrupts, but probs not necessary. nothing should run during this time?
{
    SIG_PIN->SEL0 &= ~SIG_BIT;
    SIG_PIN->SEL1 &= ~SIG_BIT;
    SIG_PIN->DIR  |= SIG_BIT;
    SIG_PIN->OUT  &= ~SIG_BIT;
    initInterruptA2();
}

void changeDrillDir(uint8_t dir)//can be reworked to rotate backwards?? also blocks for a set duration!!
{
    if (dir == 1) {
        time = 150;
    }
    else {//0 is forward
        time = 300;
    }
}

void initInterruptA2()
{
    TIMER_A2->CCTL[0] = TIMER_A_CCTLN_CCIE; // TA1CCR0 interrupt enabled
    TIMER_A2->CCR[0] = PERIOD;

    TIMER_A2->CTL = TIMER_A_CTL_SSEL__SMCLK | // SMCLK, up mode
                TIMER_A_CTL_MC__UP |  TIMER_A_CTL_ID__8;
    __enable_irq();

    NVIC_EnableIRQ(TA2_0_IRQn);
}

void TA2_0_IRQHandler()
{
    /*Clear flag*/
    TIMER_A2->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;

    if (time != 0 && time%20 == 0) {//if runtime isn't complete and intervals of 20
        SIG_PIN->OUT |= SIG_BIT;//reverse polarity
        //time -= 1;//countdown
    }
    else {
        SIG_PIN->OUT &= ~SIG_BIT;
    }
    time -= 1;
    //TIMER_A0->CCR[0] = PERIOD;
}
