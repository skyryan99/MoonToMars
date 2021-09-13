/*
 * limitSwitch.c
 *
 *  Created on: Apr 20, 2021
 *      Author: Schuyler
 */
#include "msp.h"
#include "limitSwitch.h"
#include "motorDriver.h"

static uint8_t error = 0;//if we hit the limit switch go into error state
static volatile float pinVal = 0;
//so we can't drive motor past limit switches

/*Initializes the limit switches as input and enable interrupt for both of them.
* note both switches are on the same port so if we get an interrupt trigger on that port means one of the limit switches was
* reached does not matter witch just turn off all power and enter error state
*/

void initLimitSwitches() {
    LIM_PIN->SEL0 |= LIM_BIT;
    LIM_PIN->SEL1 |= LIM_BIT;

    ADC14->CTL0 = ADC14_CTL0_SSEL__SMCLK | ADC14_CTL0_ON | ADC14_CTL0_SHP | ADC14_CTL0_SHT0_3;
    ADC14->CTL1 = ADC14_CTL1_RES_3;// sets DF to 0!
    ADC14->MCTL[0] = ADC14_MCTLN_VRSEL_0 |  //Sett to 1110b = V(R+) = VeREF+, V(R-) = VeREF-
                                                //sset ADC14VRSEl = = V(R+) = AVCC, V(R-) = AVSS
                        ADC14_MCTLN_INCH_15; //AD15 selected (P6.0)
        //| ADC14_CTL0_ENC enable

    ADC14->CLRIFGR0 |= ADC14_CLRIFGR0_CLRIFG0;
    ADC14->IER0 = ADC14_IER0_IE0;    //eanble interupt on FL bit
    __enable_irq();
    NVIC_EnableIRQ(ADC14_IRQn);

    TIMER_A2->CCTL[0] = TIMER_A_CCTLN_CCIE; // TA2CCR0 interrupt enabled
    TIMER_A2->CCR[0] = 3000;
    TIMER_A2->CTL = TIMER_A_CTL_SSEL__SMCLK | // SMCLK, up mode
                TIMER_A_CTL_MC__UP |  TIMER_A_CTL_ID__8;
    NVIC_EnableIRQ(TA2_0_IRQn);

    error = 0; //not in error state yet
    pinVal = 0;
}

/*lets the user check if we have entered an error state*/
uint8_t getError() {
    return error;
}

/*reset error state to normal. This hopes the user knows why they entered the error state and not going to damage shit*/
void resetError() {
    error = 0;
}

float getVal() {
    return pinVal;
}

void TA2_0_IRQHandler() {

    TIMER_A2->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    TIMER_A2->CCR[0] = 3000;//2 ms
    readADC();//get new val
    if (pinVal >= 0.85) {//value can be varied along with coef and frequency of measurements to eliminate noise
        error = 1;
    }
}

void readADC(){
    ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;//triggers a call to Read ADC value
}

void ADC14_IRQHandler(){//triggers when a new value of ADC is detected
    float temp, coef = 0.96;

    ADC14->CLRIFGR0 |= ADC14_CLRIFGR0_CLRIFG0;

    temp = (((ADC14->MEM[0]) & 0x3FFF)*33)/ADCMAX;
    pinVal = coef * pinVal + temp * (1 - coef);//low pass filter
}
