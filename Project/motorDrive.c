 /*
 * motorDrive.c
 *
 *  Created on: March 1, 2021
 *      Author: Schuyler Ryan
 */
#include "msp.h"
#include "motorDrive.h"

#define PERIOD 400
#define CLOCKWISE 1
#define COUNTER 0

/*To control time of low signal and direction of signal*/
static uint16_t time;
static uint8_t dir;


/*Initializes the motor control output to outputs*/
void initMotorDrives()
{
    /*PWM remain perma-on*/
    M1_PWM_PIN->SEL0 &= ~M1_PWM;
    M1_PWM_PIN->SEL1 &= ~M1_PWM;
    M1_PWM_PIN->DIR |= M1_PWM;
    M1_PWM_PIN->OUT |= M1_PWM;
    M2_PWM_PIN->SEL0 &= ~M2_PWM;
    M2_PWM_PIN->SEL1 &= ~M2_PWM;
    M2_PWM_PIN->DIR |= M2_PWM;
    M2_PWM_PIN->OUT |= M2_PWM;

    M1_CW_PIN->SEL0 &= ~M1_CW;
    M1_CW_PIN->SEL1 &= ~M1_CW;

    M2_CW_PIN->SEL0 &= ~M2_CW;
    M2_CW_PIN->SEL1 &= ~M2_CW;

    M1_CCW_PIN->SEL0 &= ~M1_CCW;
    M1_CCW_PIN->SEL1 &= ~M1_CCW;

    M2_CCW_PIN->SEL0 &= ~M2_CCW;
    M2_CCW_PIN->SEL1 &= ~M2_CCW;

    M1_CW_PIN->DIR |= M1_CW;
    M1_CCW_PIN->DIR |= M1_CCW;

    M2_CW_PIN->DIR |= M2_CW;
    M2_CCW_PIN->DIR |= M2_CCW;

    M1_CW_PIN->OUT &= ~M1_CW;     //turn off motor
    M1_CCW_PIN->OUT &= ~M1_CCW;

    M2_CW_PIN->OUT &= ~M2_CW;     //turn off motor
    M2_CCW_PIN->OUT &= ~M2_CCW;

    initInterruptA0(PERIOD);     //init Timer A for PWM
    time = PERIOD;
}

void setSpeed(int32_t pow)
{   //Between -1000 to 1000
    float MAXPOW = 1000;

    if (pow < 0 ) {   //counter-clockwise
        M1_CW_PIN->OUT &= ~M1_CW;      //turn off clockwise rotation
        M2_CW_PIN->OUT &= ~M2_CW;
        dir = COUNTER;                  //set direction
        pow = pow * -1;  //set pow to a positive value for easier calculation
    }

    else {  //clockwise
        M1_CCW_PIN->OUT &= ~M1_CCW;     //turn off ccw rotation
        M2_CCW_PIN->OUT &= ~M2_CCW;
        dir = CLOCKWISE;
    }

    /*Saturate at MAXPOW*/
    if(pow > MAXPOW ) {
        pow = MAXPOW;
    }

    time = PERIOD - PERIOD * (pow / MAXPOW);    //turns the pow value given to get time value of off time
}

void initInterruptA0(uint32_t startVal)
{
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE; // TACCR0 interrupt enabled
    TIMER_A0->CCR[0] = startVal;

    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | // SMCLK, up mode
                TIMER_A_CTL_MC__UP |  TIMER_A_CTL_ID__8;
    __enable_irq();

    NVIC_EnableIRQ(TA0_0_IRQn);
}

void TA0_0_IRQHandler()
{
    /*Clear flag*/
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;

    /*Consider clockwise-direction*/
    if (dir == CLOCKWISE) {
        /*Turn off*/
        if (time == PERIOD) {
            M1_CW_PIN->OUT &= ~M1_CW;
            M2_CW_PIN->OUT &= ~M2_CW;
            TIMER_A0->CCR[0] = PERIOD;
            return;
        }

        /*Full power*/
        if (time == 0) {
            M1_CW_PIN->OUT |= M1_CW;
            M2_CW_PIN->OUT |= M2_CW;
            TIMER_A0->CCR[0] = PERIOD;
            return;
        }

        /*Otherwise set power accordingly*/
        if (M1_CW_PIN->OUT & M1_CW) {
            TIMER_A0->CCR[0] = PERIOD - time;
        }
        else {
            TIMER_A0->CCR[0] = time;
        }
        M1_CW_PIN->OUT ^= M1_CW;
        M2_CW_PIN->OUT ^= M2_CW;
    }

    /*Consider counterclockwise-direction*/
    else {
        /*Turn off*/
        if (time == PERIOD) {
            M1_CCW_PIN->OUT &= ~M1_CCW;
            M2_CCW_PIN->OUT &= ~M2_CCW;
            TIMER_A0->CCR[0] = PERIOD;
            return;
        }

        /*Full power*/
        if (time == 0) {
            M1_CCW_PIN->OUT |= M1_CCW;
            M2_CCW_PIN->OUT |= M2_CCW;
            TIMER_A0->CCR[0] = PERIOD;
            return;
        }

        /*Otherwise set power accordingly*/
        if (M1_CCW_PIN->OUT & M1_CCW) {
            TIMER_A0->CCR[0] = PERIOD - time;
        }
        else {
            TIMER_A0->CCR[0] = time;
        }
        M1_CCW_PIN->OUT ^= M1_CCW;
        M2_CCW_PIN->OUT ^= M2_CCW;
    }
}
