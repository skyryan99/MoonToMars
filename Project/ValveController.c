#include "msp.h"
#include <stdint.h>
#include "motorDriver.h"
#include "delay.h"
#include "ValveController.h"
/**
 * main.c
 */
void Valve1Open(void)
{
    VALVE1_PIN->SEL1 &= ~(VALVE1_BIT);                    /* Use GPIO */
    VALVE1_PIN->SEL0 &= ~(VALVE1_BIT);
    VALVE1_PIN->DIR |= VALVE1_BIT;
    VALVE1_PIN->OUT |= VALVE1_BIT; //Valve 1 controller input high
}

void Valve1Close(void)
{
    VALVE1_PIN->SEL1 &= ~(VALVE1_BIT);                    /* Use GPIO */
    VALVE1_PIN->SEL0 &= ~(VALVE1_BIT);
    VALVE1_PIN->DIR |= VALVE1_BIT;
    VALVE1_PIN->OUT &= ~(VALVE1_BIT); //Valve 1 controller input low
}

void Valve2Open(void)
{
    VALVE2_PIN->SEL1 &= ~(VALVE2_BIT);                    /* Use GPIO */
    VALVE2_PIN->SEL0 &= ~(VALVE2_BIT);
    VALVE2_PIN->DIR |= VALVE2_BIT;
    VALVE2_PIN->OUT |= VALVE2_BIT; //Valve 2 controller input high
}

void Valve2Close(void)
{
    VALVE2_PIN->SEL1 &= ~(VALVE2_BIT);                    /* Use GPIO */
    VALVE2_PIN->SEL0 &= ~(VALVE2_BIT);
    VALVE2_PIN->DIR |= VALVE2_BIT;
    VALVE2_PIN->OUT &= ~(VALVE2_BIT); //Valve 2 controller input low
}
