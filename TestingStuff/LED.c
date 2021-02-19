/*
 * LED.c
 *
 *  Created on: Feb 18, 2021
 *      Author: Schuyler
 */

#include "LED.h"

void initLEDs(void) {

   A_PIN->SEL1 &= ~(A_BIT);      // set P1.5 as GPIO
   A_PIN->SEL0 &= ~(A_BIT);      // set P1.5 as GPIO
   A_PIN->DIR  |= A_BIT;       // set P1.5 as output

   B_PIN->SEL0 &= ~(B_BIT);    //set P4.6 as GPIO
   B_PIN->SEL1 &= ~(B_BIT);    //set P4.6 as GPIO
   B_PIN->DIR |= B_BIT;        //set P4.6 as output

   while (1) {
      A_PIN->OUT |= A_BIT;    // P1.5 on
      B_PIN->OUT &= ~(B_BIT);   // P4.6 off
      delay_ms(1200);

      A_PIN->OUT &= ~(A_BIT);   // P1.5 off
      B_PIN->OUT |= B_BIT;    // P4.6 off
      delay_ms(400);
   }
}

void delay_ms(int ms) {
    int i, j;
    for (j = 0; j < ms; j++)
        for (i = 750; i > 0; i--);   // delay 1 ms (approx)
}
