#include "msp.h"
#include "motorDriver.h"
#include "delay.h"
#include "limitSwitch.h"
#include "encoder.h"
#include "LoadCellDataCollection.h"
#include <stdio.h>
#include "stepperMotor.h"
#include "augerRot.h"
#include "servoMotor.h"
#include "Thermocouple.h"

/**
 * main.c
 */

/*
 * Some info:
 * -clock_freq can be set at various values, all found in "delay.h". This variable controls how fast the microcontroller runs instructions
 * -z-axis directions can change depending on order of pins, order of wire connections, or code. This is kept track of below for clarity
 * -encoders: 7500 ticks corresponds to 1 inch. Maximum vertical range of 30 in = 225 000 ticks.
 * -stepper motor at 2000 period runs at 0.15 inches / sec. To move 11.5in -> run for 77 seconds
 */

#define UINTMAX 4294967295
#define INT32MAX 2147483647
#define MAXLOAD 16
//33.72 is max load (150N) in lbs. 18/16 is a magic number. Giving some breathing room for motor response time

static uint32_t clock_freq;
static uint8_t emergency;//0: no emergency, 1: in emergency state, not yet addressed


void initState(void)
{
    clock_freq = FREQ_24MHZ;
    set_DCO(clock_freq);
    initLoadCells();
    initZMotorDrives();
    initZMotorEncoders();
    initStepperMotor();
    initLimitSwitches();
    initAugerRotMotor();
    initServoMotor();
    initThermocouple();
}

void emergencyState(void)
{
    if (emergency) {
        //do something
        //printf("EMERGENCY\n");//for some reason sometimes it just randomly goes into emergency state. Need to check if this is vibration on limit switch or software.
        emergency = 0;
    }

    /*Check limit switches*/
    if (getError()) {
        setDrillSpeed(0);
        setAugerSpeed(0);
        //stop stepper motor;
        resetError();
        emergency = 1;
    }
}

/* FOR USE WHEN ENCODERS ARE TURNED OFF. COUNTS IN RANDOM AMOUNTS OF TIME "i"
uint32_t drillControlLoop(uint32_t i)
{
    float WOB = readLoadCells();

    if (WOB >= MAXLOAD) {//values passed to "readLoadCells()" are irrelevant and function can be changed later to reflect
        setDrillSpeed(0);//max load reached, stop downward rot
    }
    else {
        setDrillSpeed(1000);//load is less than MAXLOAD, so drill max downward
        i++;
    }
    printf("%.2f\n", WOB);
    return i;
}
*/

void drillControlLoop(void)
{
    float WOB = readLoadCells();

    if (WOB >= MAXLOAD) {//values passed to "readLoadCells()" are irrelevant and function can be changed later to reflect
        setDrillSpeed(0);//max load reached, stop downward rot
    }
    else {
        setDrillSpeed(1000);//load is less than MAXLOAD, so drill max downward
    }
    printf("%.2f %d\n", WOB, getValDrill());
}

void main(void)
    {
    uint32_t i = 0;
    uint32_t val = 0;
    //drill down is positive
    //auger down is positive

    initState();

    //setDrillSpeed(-1000);
    //delay_ms(45000, clock_freq);

    //TESTING PRECISION OF STEPPER MOTORS. MOVED SIDE TO SIDE EQUAL TIMING TWICE. BOTH TIMES LANDED EXACTLY BACK ON START POSITION. THAT MEANS WE CAN RELIABLY USE
    //TIME TO CONTROL X-AXIS MOTION. LIMIT SWITCHES CAN BE PLACED FOR SAFETY PURPOSES ONLY.
    /*
    MoveLeft(46000);
    delay_ms(47000, clock_freq);
    MoveRight(46000);
    delay_ms(47000, clock_freq);
    MoveLeft(46000);
    delay_ms(47000, clock_freq);
    MoveRight(46000);
    delay_ms(56000, clock_freq);
    */


    //****************

    //To test how many encoder readings are lost in 100,000 runs of readLoadCells();
    //      FOUND TO BE ABOUT 1 inch in every 30 inches. Or 250/7500 per inch. Not a concern.

    //Also Testing if can successfully re-zero after the test by reversing until encoder reads 0.
    //      FOUND TO BE A BIT UNDER 1/2 inch not long enough going from 55505->0. Or 1/2inch per 7.5 inches. To go down 30 inches would be off by 2 inches. More of an issue.
    // For instance:
    // If we set machine to run until encoder reads 225 000. It will go about 31 inches instead of 30. Also it will go up 29 instead of 30, therefore leaving it 2 inches lower than it started!!
    /*
    while (1) {
        setDrillSpeed(1000);
        for (i = 0; i < 100000; i++) {//100 000 load cell readings vs 1 mil doing nothing.
            readLoadCells();
        }

        break;
    }

    setDrillSpeed(0);
    //CHECK WHAT ENCODERS READ HERE
    printf("Total: %d\n", getValDrill());

    //now reverse unimpeded
    setDrillSpeed(-1000);

    while (getValDrill() > 0) {
        ;
    }

    setDrillSpeed(0);
    printf("Finished: %d\n", getValDrill());

    //stuck forever
    while (1) {
        ;
    }
    */

    //MoveLeft(77000);//77 seconds

    //while(1) {
        //to test servo independant of other loop
        //changeDrillDir(1);//CURRENTLY NOT WORKING, VERY LIKELY A SIGNLA TIMING ISSUE. tested with LED->works. does not work with servo motor
        //delay_ms(100000, clock_freq);
    //}


    //DRILL TEST. Go APPROXIMATELY 11 inches (~80 000 on the encoder). Modulate on and off based on WOB exceeding 18lbf.
    //RUN A SECOND TEST BUT: Turn on printf() for WOB, but dont rely on encoders (thet get fucked up by print statements).
    //MoveRight(17000);

    while (1) {

        /*Check emergency conditions*/
        emergencyState();

        //NEED TO DEBUG THIS. ONLY WORKS WHEN PRINT STATEMENT ADDED. OBV WILL MESS UP ENCODER VALS THAT WAY.

        while (getValDrill() < 650) {//check encoders to verify downward movement. 7500 ticks = 1in. WHEN PRINTING DATA, NEW CONVERSION USED. ~35.85 ticks = 1 in.
            //while (i < 400) {//check encoders to verify downward movement. 7500 ticks = 1in, so 13.67 in = 102 500 ticks.
            drillControlLoop();
            emergencyState();//a good idea to get in practice of running alongside drill movement to check limit switches
        }

        changeDrillDir(1);//1 puts it in reverse, 0 puts it in forward. runs servo motor. This is just for show rn.
        setDrillSpeed(0);
        printf("DONE! Going up in 20 sec...\n");
        delay_ms(20000, clock_freq);//delay for 20 seconds to give operator a chance to change drill dir manually and take measurement if needed

        do {//check encoders to see if position has been returned to zero
            //while (i > 0) {//check encoders to see if position has been returned to zero
            val = getValDrill();
            setDrillSpeed(-1000);//max upward
            emergencyState();
            //i--;
            printf("Going up! - %d\n", val);
        } while (val > 0);

        setDrillSpeed(0);//halt when returned to position, also adjust servo to reset forward
        changeDrillDir(0);//return servo spin to other direction. AGAIN, CHANGE MANUALLY FOR NOW.
        printf("\nDone!\n");
        break;
    }

    MoveLeft(46000);
    delay_ms(18000, clock_freq);
    MoveRight(46000);

    while (1) {
        ;
    }

    /*while (1) {
        setDrillSpeed(-1000);
        if (getValDrill() >= 10000) {
            setDrillSpeed(0);
            setValDrill(0);
            delay_ms(1500, clock_freq);
        }
    }
    */

    //setDrillSpeed(1000);
    //delay_ms(100, clock_freq);
    //setAugerSpeed(-1000);

    //while (1) {
        //setDrillSpeed(-1000);  //100% down
        //delay_ms(3000, clock_freq);   //10s

        //setDrillSpeed(1000);   //100% up
        //delay_ms(3000, clock_freq);    //10s
        //setDrillSpeed(0);

        //if (newValDrill())
            //printf("%d\n", getValDrill());

        //setAugerSpeed(-1000);
        //delay_ms(3000, clock_freq);

        //setAugerSpeed(1000);
        //delay_ms(3000, clock_freq);
        //setAugerSpeed(0);

        //readLoadCells(thresholdDown, DOWN);
        //delay_ms(UINTMAX, clock_freq);
        //readLoadCells(thresholdUp, UP);
    //}

    //****************

    //augerRotMotorOn();
    //delay_ms(3000, clock_freq);
    //augerRotMotorOff();
    //delay_ms(3000, clock_freq);


    /*augerRotMotorOn();
    delay_ms(50, clock_freq);
    augerRotMotorOff();
    delay_ms(1500, clock_freq);

    augerRotMotorOn();
    delay_ms(10, clock_freq);
    augerRotMotorOff();
    delay_ms(1500, clock_freq);
    */
}
