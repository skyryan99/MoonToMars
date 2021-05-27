/*
 * encoder.c
 *
 *  Created on: February 21, 2021
 *      Author: Schuyler Ryan
 */

#include <stdio.h>

#include "encoder.h"
#include "msp.h"

static volatile int32_t valDrill = 0;//motor encoder value
static volatile enum DIR dirDrill;//motor direction
static volatile int32_t valAug;//motor encoder value
static volatile enum DIR dirAug;//motor direction
static volatile uint8_t updateDrill;//if there is a new motor value to display
static volatile uint8_t updateAug;//if there is a new motor value to display
/*initializes the 2 motor encoder input channels to inputs and enables interrupt for the two pins
*/
void initZMotorEncoders() {
    DRILL_A_PIN->SEL0 &= ~DRILL_A_BIT;
    DRILL_A_PIN->SEL1 &= ~DRILL_A_BIT;
    DRILL_A_PIN->DIR  &= ~DRILL_A_BIT;
    DRILL_B_PIN->SEL0 &= ~DRILL_B_BIT;
    DRILL_B_PIN->SEL1 &= ~DRILL_B_BIT;
    DRILL_B_PIN->DIR  &= ~DRILL_B_BIT;

    AUG_A_PIN->SEL0 &= ~AUG_A_BIT;
    AUG_A_PIN->SEL1 &= ~AUG_A_BIT;
    AUG_A_PIN->DIR  &= ~AUG_A_BIT;
    AUG_B_PIN->SEL0 &= ~AUG_B_BIT;
    AUG_B_PIN->SEL1 &= ~AUG_B_BIT;
    AUG_B_PIN->DIR  &= ~AUG_B_BIT;

    NVIC_EnableIRQ(PORT3_IRQn);//need to change if change encoder pins
    NVIC_EnableIRQ(PORT4_IRQn);//need to change if change encoder pins

    DRILL_A_PIN->IFG &= ~DRILL_A_BIT;//start with interrupts cleared
    DRILL_B_PIN->IFG &= ~DRILL_B_BIT;
    AUG_A_PIN->IFG &= ~AUG_A_BIT;
    AUG_B_PIN->IFG &= ~AUG_B_BIT;

    DRILL_A_PIN->IE |= DRILL_A_BIT;
    DRILL_B_PIN->IE |= DRILL_B_BIT;
    DRILL_A_PIN->IES &= ~DRILL_A_BIT;//rising edge
    DRILL_B_PIN->IES &= ~DRILL_B_BIT;//rising edge

    AUG_A_PIN->IE |= AUG_A_BIT;
    AUG_B_PIN->IE |= AUG_B_BIT;
    AUG_A_PIN->IES &= ~AUG_A_BIT;//rising edge
    AUG_B_PIN->IES &= ~AUG_B_BIT;//rising edge

    dirDrill = CLOCKWISE;
    dirAug = CLOCKWISE;
    updateDrill = 0;
    updateAug = 0;
    valDrill = 0;//motor default value is 0
    valAug = 0;
}

/*Set the motor encoder to a new value
*/
void setValDrill(int32_t newVal){
    valDrill = newVal;
}

/*gets the current direction of the motor
*/
enum DIR getDirDrill(){
    return dirDrill;
}

/*gets the current value of the motor
*/
int32_t getValDrill(){
    //printf("Enc: %d\n", valDrill);
    return valDrill;
}

/*Checks if there is a new value of the motor encoder
*/
uint8_t newValDrill ()
{
    if(updateDrill) {
        updateDrill = 0;
        return 1;
    }
    return 0;
}

/*Set the motor encoder to a new value
*/
void setValAug(int32_t newVal){
    valAug = newVal;
}

/*gets the current direction of the motor
*/
enum DIR getDirAug(){
    return dirAug;
}

/*gets the current value of the motor
*/
int32_t getValAug(){
    return valAug;
}

/*Checks if there is a new value of the motor encoder
*/
uint8_t newValAug ()
{
    if(updateAug) {
        updateAug = 0;
        return 1;
    }
    return 0;
}
/*If the motor channel A pin changed value.
* Interrupt if channel A of the motor encoder changes.
* depending on weather we are here because of a falling edge or a rising edge on the sensor means a different direction
* The direction is also dependent on what is the value of channel B
*/
void drillAPin() {
    if(DRILL_A_PIN->IES & DRILL_A_BIT){//here because falling edge
        if(DRILL_B_PIN->IN & DRILL_B_BIT){//B HIGH
            //printf("AvB\n");
            dirDrill = CLOCKWISE;
        }else{//B Low
            //printf("Av~B\n");
            dirDrill = COUNTERCLOCKWISE;
        }
    }else{//here b/c rising edge
        if(DRILL_B_PIN->IN & DRILL_B_BIT){//B HIGH
            //printf("A^B\n");
            dirDrill = COUNTERCLOCKWISE;
        }else{//B Low
            //printf("AvB\n");
            dirDrill = CLOCKWISE;
        }
    }
    DRILL_A_PIN->IES ^= DRILL_A_BIT;//change channel A to toggle a falling edge or a rising edge to trigger at. Do this to catch any change
}
/*If the motor channel B pin changed value.
* Interrupt if channel B of the motor encoder changes.
* depending on weather we are here because of a falling edge or a rising edge on the sensor means a different direction
* The direction is also dependent on what is the value of channel A
*/
void drillBPin() {
    if(DRILL_B_PIN->IES & DRILL_B_BIT){//here becauese falling edge
        if(DRILL_A_PIN->IN & DRILL_A_BIT){//A HIGH
            //printf("BvA\n");
            dirDrill = COUNTERCLOCKWISE;
        }else{//A Low
            //printf("Bv~A\n");
            dirDrill = CLOCKWISE;
        }
    }else{//here b/c rising edge
        if(DRILL_A_PIN->IN & DRILL_A_BIT){//A HIGH
            //printf("B^A\n");
            dirDrill = CLOCKWISE;
        }else{//A Low
            //printf("B^~A\n");
            dirDrill = COUNTERCLOCKWISE;
        }
    }
    DRILL_B_PIN->IES ^= DRILL_B_BIT;//change channel B to toggle a falling edge or a rising edge to trigger at. Do this to catch any change
}

void augAPin() {
    if (AUG_A_PIN->IES & AUG_A_BIT) {//here becauese falling edge
        if (AUG_B_PIN->IN & AUG_B_BIT) {//B HIGH
            dirAug = CLOCKWISE;
        } else {//B Low
            dirAug = COUNTERCLOCKWISE;
        }
    } else {//here b/c rising edge
        if (AUG_B_PIN->IN & AUG_B_BIT) {//B HIGH
            dirAug = COUNTERCLOCKWISE;
        } else {//B Low
            dirAug = CLOCKWISE;
        }
    }
    AUG_A_PIN->IES ^= AUG_A_BIT;//change channel A to toggle a falling edge or a rising edge to trigger at. Do this to catch any change
}
/*If the motor channel B pin changed value. Same type of code to the angle sensor just inverted.
* Interrupt if channel B of the motor encoder changes.
* depending on weather we are here becauese of a falling edge or a rising edge on the sensor means a different direction
* The direction is also dependent on what is the value of channel A
*/
void augBPin() {
    if(AUG_B_PIN->IES & AUG_B_BIT){//here becauese falling edge
        if(AUG_A_PIN->IN & AUG_A_BIT){//A HIGH
            dirAug = COUNTERCLOCKWISE;
        }else{//A Low
            dirAug = CLOCKWISE;
        }
    }else{//here b/c rising edge
        if(AUG_A_PIN->IN & AUG_A_BIT){//B HIGH
            dirAug = CLOCKWISE;
        }else{//B Low
            dirAug = COUNTERCLOCKWISE;
        }
    }
    AUG_B_PIN->IES ^= AUG_B_BIT;//change channel B to toggle a falling edge or a rising edge to trigger at. Do this to catch any change
}

void PORT3_IRQHandler() {//for DRILL_A_PIN and DRILL_B_PIN
    //printf("Drill enc interrupt\n");
    if (DRILL_A_PIN->IFG & DRILL_A_BIT) {//if the motor encoder channel A changed or channel B changed
        DRILL_A_PIN->IFG &= ~DRILL_A_BIT;
        drillAPin();
    }
    else if (DRILL_B_PIN->IFG & DRILL_B_BIT) {
        DRILL_B_PIN->IFG &= ~DRILL_B_BIT;
        drillBPin();
    }
    valDrill += (dirDrill - 1);//Clockwise is 0 and COUNTERCLOCKWISE is 2
    updateDrill = 1;//there is a new value to read from the motor encoder
    dirDrill = NOUPDATE;
}

void PORT4_IRQHandler() {//for AUG_A_PIN and AUG_B_PIN
    //printf("Auger enc interrupt\n");
    if(AUG_A_PIN->IFG & AUG_A_BIT){//if the motor encoder channel A changed or channel B changed
        AUG_A_PIN->IFG &= ~AUG_A_BIT;
        augAPin();
    }
    else if (AUG_B_PIN->IFG & AUG_B_BIT) {
        AUG_B_PIN->IFG &= ~AUG_B_BIT;
        augBPin();
    }
    valAug += (dirAug - 1);//Clockwise is 0 and COUNTERCLOCKWISE is 2
    updateAug = 1;//there is a new value to read from the motor encoder
    dirAug = NOUPDATE;
}
