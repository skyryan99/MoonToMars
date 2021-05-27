#include <stdio.h>
#include "msp.h"
#include "motorDriver.h"
#include "delay.h"
#include "limitSwitch.h"
#include "encoder.h"
#include "LoadCellDataCollection.h"
#include "stepperMotor.h"
#include "augerRot.h"
#include "servoMotor.h"
#include "HeaterController.h"
#include "PumpController.h"
#include "Thermocouple.h"
#include "ValveController.h"

/**
 * main.c
 */

/*
 * Some info:
 * -clock_freq can be set at various values, all found in "delay.h". This variable controls how fast the microcontroller runs instructions
 * -z-axis directions can change depending on order of pins, order of wire connections, or code. This is kept track of below for clarity
 * -encoders: 7500 ticks corresponds to 1  without any blocking. Maximum vertical range of 30 in = 225 000 ticks. Test for true values
 * -stepper motor at 2000 period runs at 0.15 inches / sec. To move 7.5in -> run for 50 seconds. Quite precise, can rely on timing for distance
 * -readLoadCells(), forwardServo(), reverseServo() all block for a small amount
 */

typedef enum {
    DRILLDOWN, DRILLUP, CHANGETOOL, AUGERDOWN, MELTING, AUGERUP
}STATE;

#define UINTMAX 4294967295
#define INT32MAX 2147483647
#define MAXLOAD 15
//33.72 is max load (150N) in lbs. 18/16 is a magic number. Giving some breathing room for motor response time

static uint32_t clock_freq = FREQ_24MHZ;
static uint8_t emergency;//0: no emergency, 1: in emergency state, not yet addressed
static STATE state;//keeps track of state for emergencies


void initState(void)
{
    set_DCO(clock_freq);
    initLoadCells();
    //printf("Init Load Cells\n");
    initZMotorDrives();
    //printf("Init Z-Motor Drives\n");
    initZMotorEncoders();
    //printf("Init Z-Motor Encoders\n");
    initStepperMotor();
    //printf("Init Stepper Motors\n");
    initLimitSwitches();
    //printf("Init Limit Switches\n");
    initAugerRotMotor();
    //printf("Init Auger Rotating Motor\n");
    initServoMotor();
    //printf("Init Servo Motor\n");
    //INIT STUFF FROM HEATER, VALVES, THERMOCOUPLE, PUMP
}



void emergencyState(void)
{
    if (emergency) {
        setDrillSpeed(0);//stops drill
        setAugerSpeed(0);//stops auger
        killStepperMotor();//stops stepper
        //do something based on STATE

        //printf("EMERGENCY\n");//for some reason sometimes it just randomly goes into emergency state. Need to check if this is vibration on limit switch or software.
        emergency = 0;
    }

    //Check limit switches. LATER ADD OTHER CAUSES OF EMERGENCY
    if (getError()) {
        resetError();
        emergency = 1;
    }
}

void drillControlLoop(void)
{
    float WOB = readLoadCells();

    if (WOB >= MAXLOAD) {//values passed to "readLoadCells()" are irrelevant and function can be changed later to reflect
        setDrillSpeed(0);//max load reached, stop downward rot
    }
    else {
        setDrillSpeed(1000);//load is less than MAXLOAD, so drill max downward
    }
    //printf("%.2f %d\n", WOB, getValDrill());
}

void servo()
{
    //printf("I'm here!\n");

    drillOn();
    delay_ms(1000, clock_freq);
    drillOff();

    while (1) {
        ;
    }
}

void main(void)
{
    int32_t val = 0;

    //drill down is positive
    //auger down is positive

    initState();


    //To test how many encoder readings are lost in 100,000 runs of readLoadCells();
    //      FOUND TO BE ABOUT 1 inch in every 30 inches. Or 250/7500 per inch. Not a concern.

    //Also Testing if can successfully re-zero after the test by reversing until encoder reads 0.
    //      FOUND TO BE A BIT UNDER 1/2 inch not long enough going from 55505->0. Or 1/2inch per 7.5 inches. To go down 30 inches would be off by 2 inches. More of an issue.
    // For instance:
    // If we set machine to run until encoder reads 225 000. It will go about 31 inches instead of 30. Also it will go up 29 instead of 30, therefore leaving it 2 inches lower than it started!!

    while (1) {

        //Servo motors testing
        servo();

        //Check emergency conditions
        emergencyState();

        //NEED TO DEBUG THIS. ONLY WORKS WHEN PRINT STATEMENT ADDED. OBV WILL MESS UP ENCODER VALS THAT WAY.

        //Drill downward state
        do {//check encoders to verify downward movement. 7500 ticks = 1in. WHEN PRINTING DATA, NEW CONVERSION USED. ~35.85 ticks = 1 in.
            val = getValDrill();
            drillControlLoop();
            if (val > 200) {//turn drill on after 200 encoders
                drillOn();
            }
            emergencyState();//a good idea to get in practice of running alongside drill movement to check limit switches
        } while (val < 650);

        reverseServo(clock_freq);//reverses servo motor
        setDrillSpeed(0);//stop it translating for a bit
        drillOff();//stop it spinning for a bit
        printf("DONE! Going up in 20 sec...\n");
        delay_ms(20000, clock_freq);//delay for 20 seconds to give operator a chance to change drill dir manually and take measurement if needed

        do {//check encoders to see if position has been returned to zero
            val = getValDrill();
            setDrillSpeed(-1000);//max upward
            emergencyState();
            //printf("Going up! - %d\n", val);//fluff printf added to match the printf in the drillControlLoop
        } while (val > 0);

        setDrillSpeed(0);//halt when returned to position, also adjust servo to reset forward
        forwardServo(clock_freq);//return servo spin to other direction. AGAIN, CHANGE MANUALLY FOR NOW.
        printf("\nDone!\n");
        break;
    }

    MoveLeft(50000);
    delay_ms(50000, clock_freq);

    MoveRight(50000);//this right movement is just to reset. normally it will not run because auger function will go

    while (1) {//block forever
        ;
    }
}
