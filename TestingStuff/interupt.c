/*interupt.c*/
#include "interupt.h"
#include "msp.h"
void initInterupA1(uint32_t startVal){
    TIMER_A1->CCTL[0] = TIMER_A_CCTLN_CCIE; // TACCR0 interrupt enabled
    TIMER_A1->CCR[0] = startVal;
    TIMER_A1->CTL = TIMER_A_CTL_SSEL__SMCLK | // SMCLK, up mode
            TIMER_A_CTL_MC__UP |  TIMER_A_CTL_ID__8;


    __enable_irq();

    NVIC_EnableIRQ(TA1_0_IRQn);

}

void initInterupA0(uint32_t startVal){
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE; // TACCR0 interrupt enabled
    TIMER_A0->CCR[0] = startVal;

    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | // SMCLK, up mode
                TIMER_A_CTL_MC__UP |  TIMER_A_CTL_ID__8;
    __enable_irq();

    NVIC_EnableIRQ(TA0_0_IRQn);

}
