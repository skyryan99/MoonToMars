#include "msp.h"
#include <stdint.h>
#include "HeaterController.h"

/**
 * main.c
 */
void Heater1Off(void)
{
    HEATER1_PIN->SEL1 &= ~(HEATER1_BIT);                    /* Use GPIO */
    HEATER1_PIN->SEL0 &= ~(HEATER1_BIT);
    HEATER1_PIN->DIR |= HEATER1_BIT;
    HEATER1_PIN->OUT &= ~(HEATER1_BIT);
}

void Heater1On(void)
{
    HEATER1_PIN->SEL1 &= ~(HEATER1_BIT);                    /* Use GPIO */
    HEATER1_PIN->SEL0 &= ~(HEATER1_BIT);
    HEATER1_PIN->DIR |= HEATER1_BIT;
    HEATER1_PIN->OUT |= HEATER1_BIT;
}

void Heater2Off(void)
{
    HEATER2_PIN->SEL1 &= ~(HEATER2_BIT);                    /* Use GPIO */
    HEATER2_PIN->SEL0 &= ~(HEATER2_BIT);
    HEATER2_PIN->DIR |= HEATER2_BIT;
    HEATER2_PIN->OUT &= ~(HEATER2_BIT);
}

void Heater2On(void)
{
    HEATER2_PIN->SEL1 &= ~(HEATER2_BIT);                    /* Use GPIO */
    HEATER2_PIN->SEL0 &= ~(HEATER2_BIT);
    HEATER2_PIN->DIR |= HEATER2_BIT;
    HEATER2_PIN->OUT |= HEATER2_BIT;
}
