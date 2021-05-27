 /*
 * augerRot.c
 *
 *  Created on: March 1, 2021
 *      Author: Schuyler Ryan
 */
#include "msp.h"
#include "augerRot.h"

#define PERIOD 400
#define CLOCKWISE 1
#define COUNTER 0

static uint16_t time;//To control time of low signal and direction of signal
static uint8_t dir;//direction of motion for auger


/*Initializes the motor control output to outputs*/
void initAugerRotMotor()
{
    /*PWM remain perma-on*/
    PWM_PIN->SEL0 &= ~AUG_PWM;
    PWM_PIN->SEL1 &= ~AUG_PWM;
    PWM_PIN->DIR |= AUG_PWM;
    PWM_PIN->OUT |= AUG_PWM;

    /*CW and CCW direction pins set for output*/
    CW_PIN->SEL0 &= ~AUG_CW;
    CW_PIN->SEL1 &= ~AUG_CW;

    CCW_PIN->SEL0 &= ~AUG_CCW;
    CCW_PIN->SEL1 &= ~AUG_CCW;

    CW_PIN->DIR |= AUG_CW;
    CCW_PIN->DIR |= AUG_CCW;

    CW_PIN->OUT &= ~AUG_CW;     //turn off motor
    CCW_PIN->OUT &= ~AUG_CCW;

    initInterruptA3(PERIOD);     //init Timer A for PWM
    time = PERIOD;
}

void setAugerRotSpeed(int32_t pow)
{   //Between -1000 to 1000
    float MAXPOW = 1000;

    if (pow < 0 ) {   //counter-clockwise
        CW_PIN->OUT &= ~AUG_CW;
        dir = COUNTER;                  //set direction
        pow = pow * -1;  //set pow to a positive value for easier calculation
    }

    else {  //clockwise
        CCW_PIN->OUT &= ~AUG_CCW;
        dir = CLOCKWISE;
    }

    /*Saturate at MAXPOW*/
    if(pow > MAXPOW ) {
        pow = MAXPOW;
    }

    time = PERIOD - PERIOD * (pow / MAXPOW);    //turns the pow value given to get time value of off time
}

void initInterruptA3(uint32_t startVal)
{
    TIMER_A3->CCTL[0] = TIMER_A_CCTLN_CCIE; // TA0CCR0 interrupt enabled
    TIMER_A3->CCR[0] = startVal;

    TIMER_A3->CTL = TIMER_A_CTL_SSEL__SMCLK | // SMCLK, up mode
                TIMER_A_CTL_MC__UP |  TIMER_A_CTL_ID__8;
    __enable_irq();

    NVIC_EnableIRQ(TA3_0_IRQn);
}

void TA3_0_IRQHandler()
{
    /*Clear flag*/
    TIMER_A3->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    //printf("Auger rot interrupt\n");

    /*Consider clockwise-direction*/
    if (dir == CLOCKWISE) {
           /*Turn off*/
           if (time == PERIOD) {
               CW_PIN->OUT &= ~AUG_CW;
               TIMER_A3->CCR[0] = PERIOD;
               return;
           }

           /*Full power*/
           if (time == 0) {
               CW_PIN->OUT |= AUG_CW;
               TIMER_A3->CCR[0] = PERIOD;
               return;
           }

           /*Otherwise set power accordingly*/
           if (CW_PIN->OUT & AUG_CW) {
               TIMER_A3->CCR[0] = PERIOD - time;
           }
           else {
               TIMER_A3->CCR[0] = time;
           }
           CW_PIN->OUT ^= AUG_CW;
       }

       /*Consider counterclockwise-direction*/
       else {
           /*Turn off*/
           if (time == PERIOD) {
               CCW_PIN->OUT &= ~AUG_CCW;
               TIMER_A3->CCR[0] = PERIOD;
               return;
           }

           /*Full power*/
           if (time == 0) {
               CCW_PIN->OUT |= AUG_CCW;
               TIMER_A3->CCR[0] = PERIOD;
               return;
           }

           /*Otherwise set power accordingly*/
           if (CCW_PIN->OUT & AUG_CCW) {
               TIMER_A3->CCR[0] = PERIOD - time;
           }
           else {
               TIMER_A3->CCR[0] = time;
           }
           CCW_PIN->OUT ^= AUG_CCW;
       }
}
