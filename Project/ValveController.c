#include "msp.h"
#include <stdint.h>
#include "motorDriver.h"
#include "ValveController.h"
#include "delay.h"
/**
 * main.c
 */
void initValves() {
    VALVE1_PIN->SEL1 &= ~(VALVE1_BIT);                    /* Use GPIO */
    VALVE1_PIN->SEL0 &= ~(VALVE1_BIT);
    VALVE1_PIN->DIR |= VALVE1_BIT;

    VALVE2_PIN->SEL1 &= ~(VALVE2_BIT);                    /* Use GPIO */
    VALVE2_PIN->SEL0 &= ~(VALVE2_BIT);
    VALVE2_PIN->DIR |= VALVE2_BIT;

    VALVE3_PIN->SEL1 &= ~(VALVE3_BIT);                    /* Use GPIO */
    VALVE3_PIN->SEL0 &= ~(VALVE3_BIT);
    VALVE3_PIN->DIR |= VALVE3_BIT;

    Valve1Close();
    Valve2Close();
    Valve3Close();
}

void Valve1Open(void)
{
    VALVE1_PIN->OUT |= VALVE1_BIT; //Valve 1 controller input high
    delay_ms(DELAY, 24);//delay to enable valve to actuate
}

void Valve1Close(void)
{
    VALVE1_PIN->OUT &= ~(VALVE1_BIT); //Valve 1 controller input low
    delay_ms(DELAY, 24);//delay to enable valve to actuate
}

void Valve2Open(void)
{
    VALVE2_PIN->OUT |= VALVE2_BIT; //Valve 2 controller input high
    delay_ms(DELAY, 24);//delay to enable valve to actuate
}

void Valve2Close(void)
{
    VALVE2_PIN->OUT &= ~(VALVE2_BIT); //Valve 2 controller input low
    delay_ms(DELAY, 24);//delay to enable valve to actuate
}

void Valve3Open(void)
{
    VALVE3_PIN->OUT |= VALVE3_BIT; //Valve 2 controller input high
    delay_ms(DELAY, 24);//delay to enable valve to actuate
}

void Valve3Close(void)
{
    VALVE3_PIN->OUT &= ~(VALVE3_BIT); //Valve 2 controller input low
    delay_ms(DELAY, 24);//delay to enable valve to actuate
}
