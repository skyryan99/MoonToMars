#include "msp.h"
#include <stdint.h>
#include "motorDriver.h"
#include "delay.h"
#include "stepperMotor.h"

#define PERIOD 2000
//decreasing this speeds up rotation - to a point. Do not reduce below 1000 or cause fire alarm. Do not exceed 3000 or cause earthquakes.
//Timing numbers are (N/3000)*ms.

static uint32_t time;//time for motor to run

void initStepperMotor()
{
    DIR_PIN->SEL1 &= ~(DIR_BIT);                    /* Use GPIO */
    DIR_PIN->SEL0 &= ~(DIR_BIT);
    DIR_PIN->DIR |= DIR_BIT;
    PUL_PIN->SEL1 &= ~(PUL_BIT);
    PUL_PIN->SEL0 &= ~(PUL_BIT);
    PUL_PIN->DIR |= PUL_BIT;
    ENA_PIN->SEL1 &= ~(ENA_BIT);
    ENA_PIN->SEL0 &= ~(ENA_BIT);
    ENA_PIN->DIR |= ENA_BIT;
    ENA_PIN->OUT &= ~(ENA_BIT);

    initInterruptA1();
}

//runtime is in milliseconds. irq is every 1/8ms
void MoveLeft(uint32_t runtime)
{
    DIR_PIN->OUT |= DIR_BIT;
    time = (3000 * runtime) / PERIOD;
}

void MoveRight(uint32_t runtime)
{
    DIR_PIN->OUT &= ~DIR_BIT;
    time = (3000 * runtime) / PERIOD;
}

void killStepperMotor()
{
    time = 0;
}

void initInterruptA1()
{
    TIMER_A1->CCTL[0] = TIMER_A_CCTLN_CCIE; // TA1CCR0 interrupt enabled
    TIMER_A1->CCR[0] = PERIOD;

    TIMER_A1->CTL = TIMER_A_CTL_SSEL__SMCLK | // SMCLK, up mode
                TIMER_A_CTL_MC__UP |  TIMER_A_CTL_ID__8;
    __enable_irq();

    NVIC_EnableIRQ(TA1_0_IRQn);
}

void TA1_0_IRQHandler()
{
    /*Clear flag*/
    TIMER_A1->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    //printf("Stepper interrupt\n");

    if (time != 0) {//if runtime isn't complete
        PUL_PIN->OUT ^= PUL_BIT;//reverse polarity
        time -= 1;//countdown
    }

    TIMER_A1->CCR[0] = PERIOD;//added erroneously, not sure what it does
}
