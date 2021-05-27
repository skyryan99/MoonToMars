#include "msp.h"
#include <stdint.h>
#include "PumpController.h"
/**
 * main.c
 */
void PumpForward(void)
{
    PUMP1_PIN->SEL1 &= ~(PUMP1_BIT);                    /* Use GPIO */
    PUMP1_PIN->SEL0 &= ~(PUMP1_BIT);
    PUMP1_PIN->DIR |= PUMP1_BIT;
    PUMP1_PIN->OUT &= ~(PUMP1_BIT);
    PUMP2_PIN->SEL1 &= ~(PUMP2_BIT);                    /* Use GPIO */
    PUMP2_PIN->SEL0 &= ~(PUMP2_BIT);
    PUMP2_PIN->DIR |= PUMP2_BIT;
    PUMP2_PIN->OUT |= PUMP2_BIT;
}

void PumpBackward(void)
{
    PUMP1_PIN->SEL1 &= ~(PUMP1_BIT);                    /* Use GPIO */
    PUMP1_PIN->SEL0 &= ~(PUMP1_BIT);
    PUMP1_PIN->DIR |= PUMP1_BIT;
    PUMP1_PIN->OUT |= PUMP1_BIT;
    PUMP2_PIN->SEL1 &= ~(PUMP2_BIT);                    /* Use GPIO */
    PUMP2_PIN->SEL0 &= ~(PUMP2_BIT);
    PUMP2_PIN->DIR |= PUMP2_BIT;
    PUMP2_PIN->OUT &= ~(PUMP2_BIT);
}

void PumpOff(void)
{
    PUMP1_PIN->SEL1 &= ~(PUMP1_BIT);                    /* Use GPIO */
    PUMP1_PIN->SEL0 &= ~(PUMP1_BIT);
    PUMP1_PIN->DIR |= PUMP1_BIT;
    PUMP1_PIN->OUT &= ~(PUMP1_BIT);
    PUMP2_PIN->SEL1 &= ~(PUMP2_BIT);                    /* Use GPIO */
    PUMP2_PIN->SEL0 &= ~(PUMP2_BIT);
    PUMP2_PIN->DIR |= PUMP2_BIT;
    PUMP2_PIN->OUT &= ~(PUMP2_BIT);
}
