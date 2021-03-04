/*
 * motorDrive.c
 *
 *  Created on: March 1, 2021
 *      Author: Schuyler Ryan
 */
#include "msp.h"
#include "motorDrive.h"
#include "interupt.h"

#define PERIOD 300 //10k driving frequcy of the motor
#define BUFF 50//used to give interupt function enought time to run before it will trigger again
static uint16_t time;//this value is the amount of time the motor control signal is low
enum DIRMT{CCW,CW};//enum to control direction
static enum DIRMT dir;
static uint8_t error;//if we hit the limit switch go into error state so we cant drive motor past limit switches
static volatile uint32_t MAXPOW; //max power able to deliver to the motor 1000 being max and -1000 being max orther direction
/*Initilizes the limit switches as input and enable interupt for both of them. 
* note both switches are on the same port so if we get an interupt trigger on that port means one of the limit switches was
* reached does not matter witch just turn off all power and enter error state
*/
void initLimitSwitch(){
    Left_PIN->SEL0 &= ~Left_BIT;
    Left_PIN->SEL1 &= ~Left_BIT;
    Left_PIN->DIR  &= ~Left_BIT;

    Right_PIN->SEL0 &= ~Right_BIT;
    Right_PIN->SEL1 &= ~Right_BIT;
    Right_PIN->DIR  &= ~Right_BIT;

    NVIC_EnableIRQ(PORT4_IRQn);
    Left_PIN->IE |= Left_BIT;
    Right_PIN->IE |= Right_BIT;
    Left_PIN->IES |= Left_BIT;//falling edge
    Right_PIN->IES |= Right_BIT;//falling edge
    error = 0; //not in error state yet
}
/*Initlizes the motor control output to outputs. We are using an H-bridge for controing the motor. 
* Dual MC33926 Motor Driver Carrier : https://www.pololu.com/product/1213
*/
void initMotorDrive(){
    MAXPOW = 1000; 
    M1_PIN->SEL0 &=   ~M1_BIT;
    M1_PIN->SEL1 &=   ~M1_BIT;
    M1_PIN->DIR  |=    M1_BIT;
    M2_PIN->SEL0 &=   ~M2_BIT;
    M2_PIN->SEL1 &=   ~M2_BIT;
    M2_PIN->DIR  |=    M2_BIT;
    MFS_PIN->SEL0 &= ~MFS_BIT;
    MFS_PIN->SEL1 &= ~MFS_BIT;
    MFS_PIN->DIR  &= ~MFS_BIT;//not being used yet can later if need be

    M1_PIN->OUT &= ~M1_BIT;
    M2_PIN->OUT &= ~M2_BIT;//turn off motor
    initInterupA0(PERIOD); //init interupt that will driver the motors. this case is 10khz 
    time = PERIOD;//time is the low time. so set it to max time or period
    dir = CW;

    initLimitSwitch();//make sure the limit switches are running
}
/*lets the user check if we have entered an error state
*/
uint8_t getError(){
    return error;
}
/*reset error state to normal. This hopes the user knows why they entered the error state and not going to kill the cart
*/
void resetError(){
    error = 0;
}
/*used with the ADC. lets user select max power availble to the motor. in terms of percentage. So highest value is 1000 so 
* multiply 10 by the value they give to get 1000 = 10*100
*/
void setMaxPowDrive(uint32_t newPow){
    MAXPOW = 10*newPow;
    if(MAXPOW > 1000){//the user cant go above the limit. THat would break the code!
        MAXPOW = 1000;
    }
}
/*If the user was expecting to hit the left limit switch and wants to control the motor away from that.
* the left limit switch might trigger again when moving away. This is bad. so they can opt to disable triggering on 
* the left limit switch. If they do this we enable the right limit switch so no matter what there is 1 limit switch active
*/
void disableLeftError(){
    Left_PIN->IE &= ~Left_BIT;
    Right_PIN->IE |= Right_BIT;
}
/*If the user was expecting to hit the right limit switch and wants to control the motor away from that.
* the right limit switch might trigger again when moving away. This is bad. so they can opt to disable triggering on 
* the right limit switch. If they do this we enable the right limit switch so no matter what there is 1 limit switch active
*/
void disableRightError(){
    Right_PIN->IE &= ~Right_BIT;
    Left_PIN->IE |= Left_BIT;
}
/*Enables both limit switches to be trigged. should be in this state 99% of thetime
*/
void enableError(){
    Left_PIN->IE |= Left_BIT;
    Right_PIN->IE |= Right_BIT;
}
/*Set a speed to drive the motor at. the calc the new time value to drive the motor that our timer interupt uses to create
* the square wave
* (see datasheet of motor driver circit if need more explanation as to how we are controller the motor driver chip)
*/
void setSpeed(int32_t pow){//val between -1000 to 1000
    if(pow < 0 ){//if user wants to go CCW
        M1_PIN->OUT &= ~M1_BIT;//then set the motor control pin 1 to low and we will drive motor control pin 2 
        dir = CCW;
        pow = pow *-1;//set pow to a positive value for easier calculation
    }else{
        M2_PIN->OUT &= ~M2_BIT;//then set the motor control pin 2 to low and we will drive motor control pin 1 
        dir = CW; //we are going a diff direction now
    }
    if(pow > MAXPOW-BUFF){//if we are above the maximum allowabed speed just set to max
        pow = MAXPOW;
    }
    if(pow < BUFF || error){//if in error state dont change the speed. set speed to 0.
        pow = 0;
    }
    time = PERIOD - (pow*PERIOD)/MAXPOW;//turns the pow value given to get time value of off time

}
void PORT4_IRQHandler(){//one of the limit switches hit
    Left_PIN->IFG &= ~Left_BIT;
    Right_PIN->IFG &= ~Right_BIT;
    time = PERIOD;
    M1_PIN->OUT &= ~M1_BIT;
    M2_PIN->OUT &= ~M2_BIT;
    error =1;
}
void TA0_0_IRQHandler(){
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    if(dir==CW){
        if(time==PERIOD){
            M1_PIN->OUT &= ~M1_BIT;
            TIMER_A0->CCR[0] = PERIOD;
            return;
        }
        if(time == 0){
            M1_PIN->OUT |= M1_BIT;
            TIMER_A0->CCR[0] = PERIOD;
            return;
        }
        if(M1_PIN->OUT & M1_BIT){
           TIMER_A0->CCR[0] = time;
        }else
        {
           TIMER_A0->CCR[0] = PERIOD - time;
        }
        M1_PIN->OUT ^= M1_BIT;
    }else{
        if(time==PERIOD){
            M2_PIN->OUT &= ~M2_BIT;
            TIMER_A0->CCR[0] = PERIOD;
            return;
        }
        if(time == 0){
            M2_PIN->OUT |= M2_BIT;
            TIMER_A0->CCR[0] = PERIOD;
            return;
        }
        if(M2_PIN->OUT & M2_BIT){
            TIMER_A0->CCR[0] = time;
        }else
        {
            TIMER_A0->CCR[0] = PERIOD - time;
        }
        M2_PIN->OUT ^= M2_BIT;
    }
}

