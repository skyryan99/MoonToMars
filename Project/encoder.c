/*
 * encoder.c
 *
 *  Created on: February 21, 2021
 *      Author: Schuyler Ryan
 */

#include "encoder.h"
#include "msp.h"

static volatile int32_t valMot;//motor encoder value
static volatile enum DIR dirMot;//motor direction
static volatile uint8_t updateMot;//if there is a new motor value to display
/*initializes the 2 motor encoder input channels to inputs and enables interrupt for the two pins
*/
void initMotorEncoder(){
    MA_PIN->SEL0 &= ~MA_BIT;
    MA_PIN->SEL1 &= ~MA_BIT;
    MA_PIN->DIR  &= ~MA_BIT;
    MB_PIN->SEL0 &= ~MB_BIT;
    MB_PIN->SEL1 &= ~MB_BIT;
    MB_PIN->DIR  &= ~MB_BIT;
    NVIC_EnableIRQ(PORT3_IRQn);//need to change if change encoder pins
    MA_PIN->IE |= MA_BIT;
    MB_PIN->IE |= MB_BIT;
    MA_PIN->IES &= ~MA_BIT;//rising edge
    MB_PIN->IES &= ~MB_BIT;//rising edge
    dirMot = CLOCKWISE;
    updateMot = 0;
    valMot = 0;//motor default value is 0
}

/*Set the motor encoder to a new value
*/
void setValMot(int32_t newVal){
    valMot = newVal;
}

/*gets the current direction of the motor
*/
enum DIR getDirMot(){
    return dirMot;
}

/*gets the current value of the motor
*/
int32_t getValMot(){
    return valMot;
}

/*Checks if there is a new value of the motor encoder
*/
uint8_t newValMot ()
{
    if(updateMot) {
        updateMot =0;
        return 1;
    }
    return 0;
}

/*If the motor channel A pin changed value. Same type of code to the angle sensor just inverted.
* Interrupt if channel A of the motor encoder changes.
* depending on weather we are here becauese of a falling edge or a rising edge on the sensor means a different direction
* The direction is also dependent on what is the value of channel B
*/
void motorAPin() {
    MA_PIN->IFG &= ~MA_BIT;
    if(MA_PIN->IES & MA_BIT){//here becauese falling edge
        if(MB_PIN->IN & MB_BIT){//B HIGH
            dirMot = CLOCKWISE;
        }else{//B Low
            dirMot = COUNTERCLOCKWISE;
        }
    }else{//here b/c rising edge
        if(MB_PIN->IN & MB_BIT){//B HIGH
            dirMot = COUNTERCLOCKWISE;
        }else{//B Low
            dirMot = CLOCKWISE;
        }
    }
    MA_PIN->IES ^= MA_BIT;//change channel A to toggle a falling edge or a rising edge to trigger at. Do this to catch any change
}
/*If the motor channel B pin changed value. Same type of code to the angle sensor just inverted.
* Interrupt if channel B of the motor encoder changes.
* depending on weather we are here becauese of a falling edge or a rising edge on the sensor means a different direction
* The direction is also dependent on what is the value of channel A
*/
void motorBPin() {
    MB_PIN->IFG &= ~MB_BIT;
    if(MB_PIN->IES & MB_BIT){//here becauese falling edge
        if(MA_PIN->IN & MA_BIT){//A HIGH
            dirMot = COUNTERCLOCKWISE;
        }else{//A Low
            dirMot = CLOCKWISE;
        }
    }else{//here b/c rising edge
        if(MA_PIN->IN & MA_BIT){//B HIGH
            dirMot = CLOCKWISE;
        }else{//B Low
            dirMot = COUNTERCLOCKWISE;
        }
    }
    MB_PIN->IES ^= MB_BIT;//change channel B to toggle a falling edge or a rising edge to trigger at. Do this to catch any change
}

void PORT3_IRQHandler() {//for MA_PIN and MB_PIN
    if(MA_PIN->IFG & MA_BIT){//if the motor encoder channel A changed or channel B changed
        motorAPin();
    }
    else{
        motorBPin();
    }
    valMot += (dirMot - 1);//Clockwise is 0 and COUNTERCLOCKWISE is 2. so subtracting 1 lets me do quick math to add or subtract
    updateMot = 1;//there is a new value to read from the motor encoder
}
