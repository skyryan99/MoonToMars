/*
 * limitSwitch.c
 *
 *  Created on: Apr 20, 2021
 *      Author: Schuyler
 */
#include "msp.h"
#include "limitSwitch.h"
#include "motorDriver.h"

static uint8_t error;//if we hit the limit switch go into error state
//so we can't drive motor past limit switches

/*Initializes the limit switches as input and enable interrupt for both of them.
* note both switches are on the same port so if we get an interrupt trigger on that port means one of the limit switches was
* reached does not matter witch just turn off all power and enter error state
*/

void initLimitSwitches() {
    TOP_DRILL_PIN->SEL0 &= ~TOP_DRILL_BIT;
    TOP_DRILL_PIN->SEL1 &= ~TOP_DRILL_BIT;
    TOP_DRILL_PIN->DIR  &= ~TOP_DRILL_BIT;

    //BOT_DRILL_PIN->SEL0 &= ~BOT_DRILL_BIT;
    //BOT_DRILL_PIN->SEL1 &= ~BOT_DRILL_BIT;
    //BOT_DRILL_PIN->DIR  &= ~BOT_DRILL_BIT;

    //TOP_AUG_PIN->SEL0 &= ~TOP_AUG_BIT;
    //TOP_AUG_PIN->SEL1 &= ~TOP_AUG_BIT;
    //TOP_AUG_PIN->DIR  &= ~TOP_AUG_BIT;

    //BOT_AUG_PIN->SEL0 &= ~BOT_AUG_BIT;
    //BOT_AUG_PIN->SEL1 &= ~BOT_AUG_BIT;
    //BOT_AUG_PIN->DIR  &= ~BOT_AUG_BIT;

    //LEFT_PIN->SEL0 &= ~LEFT_BIT;
    //LEFT_PIN->SEL1 &= ~LEFT_BIT;
    //LEFT_PIN->DIR  &= ~LEFT_BIT;

    //RIGHT_PIN->SEL0 &= ~RIGHT_BIT;
    //RIGHT_PIN->SEL1 &= ~RIGHT_BIT;
    //RIGHT_PIN->DIR  &= ~RIGHT_BIT;

    NVIC_EnableIRQ(PORT4_IRQn);

    TOP_DRILL_PIN->IE |= TOP_DRILL_BIT;
    //BOT_DRILL_PIN->IE |= BOT_DRILL_BIT;
    //TOP_AUG_PIN->IE |= TOP_AUG_BIT;
    //BOT_AUG_PIN->IE |= BOT_AUG_BIT;
    //LEFT_PIN->IE |= LEFT_BIT;
    //RIGHT_PIN->IE |= RIGHT_BIT;

    TOP_DRILL_PIN->IES |= TOP_DRILL_BIT;//falling edges
    //BOT_DRILL_PIN->IES |= BOT_DRILL_BIT;
    //TOP_AUG_PIN->IES |= TOP_AUG_BIT;
    //BOT_AUG_PIN->IES |= BOT_AUG_BIT;
    //LEFT_PIN->IES |= LEFT_BIT;
    //RIGHT_PIN->IES |= RIGHT_BIT;

    error = 0; //not in error state yet
}

/*lets the user check if we have entered an error state*/
uint8_t getError() {
    return error;
}

/*reset error state to normal. This hopes the user knows why they entered the error state and not going to damage shit*/
void resetError() {
    error = 0;
}

void PORT4_IRQHandler() {//one of the limit switches hit
    TOP_DRILL_PIN->IFG &= ~TOP_DRILL_BIT;
    //BOT_DRILL_PIN->IFG &= ~BOT_DRILL_BIT;
    //TOP_AUG_PIN->IFG &= ~TOP_AUG_BIT;
    //BOT_AUG_PIN->IFG &= ~BOT_AUG_BIT;
    //LEFT_PIN->IFG &= ~LEFT_BIT;
    //RIGHT_PIN->IFG &= ~RIGHT_BIT;

    error = 1;
}
