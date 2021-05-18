/*
 * augerRot.c
 *
 *  Created on: May 11, 2021
 *      Author: skyry
 */

#include "msp.h"
#include "augerRot.h"

// Auger has on and off only.

void initAugerRotMotor()
{
    AUG_ROT_PIN->SEL0 &= ~AUG_ROT;
    AUG_ROT_PIN->SEL1 &= ~AUG_ROT;
    AUG_ROT_PIN->DIR |= AUG_ROT;
    AUG_ROT_PIN->OUT &= ~AUG_ROT;
    //init auger rotating motor off
}

void augerRotMotorOn()
{
    AUG_ROT_PIN->OUT |= AUG_ROT;
}

void augerRotMotorOff()
{
    AUG_ROT_PIN->OUT &= ~AUG_ROT;
}
