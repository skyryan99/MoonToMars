/*
 * motor.c
 *
 *  Created on: Feb 17, 2021
 *      Author: Schuyler
 */

#include <stdio.h>
#include "motor.h"
#include "msp.h"

static uint32_t timeLow;
static DIR dir;

void initMotor()
{
    printf("\nInitializing Motor...\n");

    /*Initialize Motor and Timer Channel A0*/
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE; // TACCR0 interrupt enabled
    TIMER_A0->CCR[0] = PERIOD;    //Initialized OFF
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | // SMCLK, up mode
        TIMER_A_CTL_MC__UP |  TIMER_A_CTL_ID__8;

    __enable_irq();
    NVIC_EnableIRQ(TA0_N_IRQn);

    A_PIN->DIR |= A_BIT;        //set dir to 1
    A_PIN->SEL0 &= ~(A_BIT);    //set sel0 to 0
    A_PIN->SEL1 &= ~(A_BIT);    //set sel1 to 0

    B_PIN->DIR |= B_BIT;        //set dir to 1
    B_PIN->SEL0 &= ~(B_BIT);    //set sel0 to 0
    B_PIN->SEL1 &= ~(B_BIT);    //set sel1 to 0

    /*Initialized to OFF*/
    timeLow = PERIOD;
    dir = CW;

}

void killMotor()
{
    timeLow = PERIOD;
}

void setDutyCycle(int32_t dutyCycle)
{
    printf("Setting Duty Cycle to: %d\n", dutyCycle);

    /*Sets duty cycle to be sent to motor at given level*/
    if (dutyCycle < 0 ) {
        /*Clear pin 1, Control Pin 2, Set Direction to CCW*/
        A_PIN->OUT &= ~(A_BIT);
        dir = CCW;
        dutyCycle *= -1;
    }
    else {
        /*Clear pin 2, Control Pin 1, Set Direction to CW*/
        B_PIN->OUT &= ~(B_BIT);
        dir = CW;
    }

    timeLow = PERIOD * (1 - (dutyCycle / DUTY_MAX));
}

void power() {
    fprintf(stderr, "setPow");
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;    //reset the flag

    if (dir == CW) {
        if (timeLow == PERIOD) {
            /*If OFF, clear pin 1 set CCR to period*/
            A_PIN->OUT &= ~(A_BIT);
            TIMER_A0->CCR[0] = PERIOD;
            return;
        }
        else if (timeLow == 0) {
            /*If ON set pin and set CCR to period*/
            A_PIN->OUT |= A_BIT;
            TIMER_A0->CCR[0] = PERIOD;
            return;
        }
        else if (A_PIN->OUT & A_BIT) {
            /*If A output is high then set timer to timeLow*/
            TIMER_A0->CCR[0] = timeLow;
        }
        else {
            /*If A output is low then set timer to PERIOD minus the low time*/
            TIMER_A0->CCR[0] = PERIOD - timeLow;
        }
        /*If we are not at max or min timeLow val then invert pin*/
        A_PIN->OUT ^= A_BIT;
    }

    else {
        if (timeLow == PERIOD) {
            /*If OFF, clear pin 1 set CCR to period*/
            B_PIN->OUT &= ~(B_BIT);
            TIMER_A0->CCR[0] = PERIOD;
            return;
         }
         else if (timeLow == 0) {
             /*If ON set pin and set CCR to period*/
             B_PIN->OUT |= B_BIT;
             TIMER_A0->CCR[0] = PERIOD;
             return;
         }
         else if (B_PIN->OUT & B_BIT) {
             /*If A output is high then set timer to timeLow*/
             TIMER_A0->CCR[0] = timeLow;
         }
         else {
             /*If A output is low then set timer to PERIOD minus the low time*/
             TIMER_A0->CCR[0] = PERIOD - timeLow;
         }
         /*If we are not at max or min timeLow val then invert pin*/
         B_PIN->OUT ^= B_BIT;
    }
}

void TA0_0_IRQHandler()
{
    fprintf(stderr, "irq\n");
    if (TIMER_A0->CCTL[0] & TIMER_A_CCTLN_CCIFG) {
        power();
    }
}
