/*
 * encoder.c
 *
 *  Created on: February 31, 2021
 *      Author: Schuyler Ryan
 */

#include "encoder.h"
#include "msp.h"

static volatile int32_t valRot; //angle encoder value
static volatile enum DIR dirRot; //the direction of the angle sensor
static volatile uint8_t updateRot;//if there is a new angle value to read
static volatile int32_t maxRot; //max value the angle sensor will read before looping
static volatile int32_t minRot;//min value the angle sensor weill read before looping


static volatile int32_t valMot;//motor encoder value
static volatile enum DIR dirMot;//motor direction
static volatile uint8_t updateMot;//if there is a new motor value to display
/*initlizies the 2 motor encoder input channels to inputs and enables interupt for the two pins
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
/*initlizes the 2 angle motor encoder input channels to inputs and enables interupts for the two pins
*/
void initRotEncoder(){
    A_PIN->SEL0 &= ~A_BIT;
    A_PIN->SEL1 &= ~A_BIT;
    A_PIN->DIR  &= ~A_BIT;
    B_PIN->SEL0 &= ~B_BIT;
    B_PIN->SEL1 &= ~B_BIT;
    B_PIN->DIR  &= ~B_BIT;
    NVIC_EnableIRQ(PORT5_IRQn);//need to change if change encoder pins
    NVIC_EnableIRQ(PORT6_IRQn);
    A_PIN->IE |= A_BIT;
    B_PIN->IE |= B_BIT;
    A_PIN->IES &= ~A_BIT;//rising edge
    B_PIN->IES &= ~B_BIT;//rising edge


    dirRot = CLOCKWISE;
    updateRot = 0;
    maxRot =719;//both these values was experimently determined
    minRot =-719;
    valRot = minRot;//set the starting value at full down or at -719 degress ( this was experimently determined)
}
/*Set the motor encoder to a new value
*/
void setValMot(int32_t newVal){
    valMot = newVal;
}
/*Set the angle encoder to a new value
*/
void setValRot(int32_t newVal){
    valRot = newVal;
}
/*gets the current direction of the motor
*/
enum DIR getDirMot(){
    return dirMot;
}
/*gets the current direction of the angle sensor
*/
enum DIR getDirRot(){
    return dirRot;
}
/*gets the current value of the motor
*/
int32_t getValMot(){
    return valMot;
}
/*gets the current value of the angle sensor
*/
int32_t getValRot(){
    return valRot;
}
/*Checks if there is a new value of the motor encoder
*/
uint8_t newValMot()
{
    if(updateMot){
        updateMot =0;
        return 1;
    }
    return 0;
}
/*Checks if there is a new value of the angle sensor encoder
*/
uint8_t newValRot()
{
    if(updateRot){
        updateRot =0;
        return 1;
    }
    return 0;
}
/*changes the new max rot value if the user wants to
*/
void setMaxRot(int32_t newmax){
    maxRot = newmax;
}
/*changes the new min rot value if the user wants to
*/
void setMinRot(int32_t newmin){
    minRot = newmin;
}
/*Interupt if chanel A of th rotation sensor changes.
* depending on weather we are here becaues of a falling edge or a rising edge on the sensor means a different direction
* The direction is also dependent on what is the value of channel B
*/
void PORT5_IRQHandler(){//for A_PIN
    A_PIN->IFG &= ~A_BIT;
    if(A_PIN->IES & A_BIT){//here becaues falling edge
        if(B_PIN->IN & B_BIT){//B HIGH
            dirRot = CLOCKWISE;
        }else{//B Low
            dirRot = COUNTERCLOCKWISE;
        }
    }else{//here b/c rising edge
        if(B_PIN->IN & B_BIT){//B HIGH
            dirRot = COUNTERCLOCKWISE;
        }else{//B Low
            dirRot = CLOCKWISE;
        }
    }
    A_PIN->IES ^= A_BIT;//change channel A to togle a falling edge or a rising edge to trigger at. Do this to catch any change
    valRot += (dirRot - 1);//Clockwise is 0 and COUNTERCLOCKWISE is 2. so subtracting 1 lets me do quick math to add or subtract
    //depending on the rotation without an extra if statment. SLICK!
	if(valRot > maxRot){
        valRot = minRot;//if we need to circle around
    }
    if(valRot < minRot){
        valRot = maxRot;//if we need to circle around
    }
    updateRot = 1;//there is a new value to read from the rotation sensor
}
/*Interupt if chanel B of th rotation sensor changes.
* depending on weather we are here becaues of a falling edge or a rising edge on the sensor means a different direction
* The direction is also dependent on what is the value of channel A
*/
void PORT6_IRQHandler(){//for B_PIN
    B_PIN->IFG &= ~B_BIT;
    if(B_PIN->IES & B_BIT){//here becaues falling edge
        if(A_PIN->IN & A_BIT){//A HIGH
            dirRot = COUNTERCLOCKWISE;
        }else{//A Low
            dirRot = CLOCKWISE;
        }
    }else{//here b/c rising edge
        if(A_PIN->IN & A_BIT){//B HIGH
            dirRot = CLOCKWISE;
        }else{//B Low
            dirRot = COUNTERCLOCKWISE;
        }
    }
    B_PIN->IES ^= B_BIT;//change channel B to togle a falling edge or a rising edge to trigger at. Do this to catch any change
    valRot += (dirRot - 1);//Clockwise is 0 and COUNTERCLOCKWISE is 2. so subtracting 1 lets me do quick math to add or subtract
    //depending on the rotation without an extra if statment. SLICK!
    if(valRot > maxRot){
        valRot = minRot;//if we need to circle around
    }
    if(valRot < minRot){
        valRot = maxRot;//if we need to circle around
    }
    updateRot = 1;//there is a new value to read from the rotation sensor
}
/*If the motor channel A pin changed value. Same type of code to the angle sensor just inverted.
* Interupt if chanel A of th motor encoder changes.
* depending on weather we are here becaues of a falling edge or a rising edge on the sensor means a different direction
* The direction is also dependent on what is the value of channel B
*/
void motorAPin(){
    MA_PIN->IFG &= ~MA_BIT;
    if(MA_PIN->IES & MA_BIT){//here becaues falling edge
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
    MA_PIN->IES ^= MA_BIT;//change channel A to togle a falling edge or a rising edge to trigger at. Do this to catch any change
}
/*If the motor channel B pin changed value. Same type of code to the angle sensor just inverted.
* Interupt if chanel B of th motor encoder changes.
* depending on weather we are here becaues of a falling edge or a rising edge on the sensor means a different direction
* The direction is also dependent on what is the value of channel A
*/
void motorBPin(){
    MB_PIN->IFG &= ~MB_BIT;
    if(MB_PIN->IES & MB_BIT){//here becaues falling edge
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
    MB_PIN->IES ^= MB_BIT;//change channel B to togle a falling edge or a rising edge to trigger at. Do this to catch any change
}

void PORT3_IRQHandler(){//for MA_PIN and MB_PIN
    if(MA_PIN->IFG & MA_BIT){//if the motor encoder channel A changed or channel B changed
        motorAPin();
    }
    else{
        motorBPin();
    }
    valMot += (dirMot - 1);//Clockwise is 0 and COUNTERCLOCKWISE is 2. so subtracting 1 lets me do quick math to add or subtract
    //depending on the rotation without an extra if statment. SLICK!
    updateMot = 1;//there is a new value to read from the motor encoder
}

