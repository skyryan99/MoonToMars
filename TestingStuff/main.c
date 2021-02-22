#include <stdio.h>
#include "msp.h"
#include "motor.h"
#include "LED.h"


/**
 * main.c - "CTRL + SPACE" for autocomplete
 */
void main(void)
{
   initLEDs();
}

void main2(void)
{
    int32_t c;

    /*Initialize*/
    initMotor();

    /*Wait for user to exit*/
    printf("\nEnter a PWM or 'q' to halt...\n");
    while (scanf("%d", &c)) {
        if (c == 'q') {
            break;
        }
        else {
            setDutyCycle(c);
        }
    }
}
