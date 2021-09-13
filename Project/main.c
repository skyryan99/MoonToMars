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
#include "melting.h"

/**
 * main.c
 */
/*
 * TO-DO LIST:
 * 1. Remove print statements
 * 3. Finish Emergency state
 * 4. Run many REAL full-system tests until satisfied
 * 5.Pack up Machine for shipping
 *
 */
/*
 * Some info:
 * -clock_freq can be set at various values, all found in "delay.h". This variable controls how fast the microcontroller runs instructions
 * -z-axis directions can change depending on order of pins, order of wire connections, or code. This is kept track of in main() for clarity
 * -encoders: 7500 ticks corresponds to 1  without any blocking theoretical. Maximum vertical range of 30 in = 225 000 ticks. Test for true values.
 *                                                                                                                  ~7250 ticks/in for true values
 * -stepper motor at 1800 period runs at 0.25 inches / sec. To move 7.5in -> run for 30 seconds. Quite precise, can rely on timing for distance
 * -readLoadCells(), forwardServo(), reverseServo(), emergencyState(), and all STATE functions block for a some amount of time
 * - readCount() in LoadCellDataCollection.c requires that no limit switches be depressed, so comment out initLoadCells() in initState if need to run
 * code while limit switch is being depressed
 */

typedef enum {
    CHANGETODRILL, DRILLDOWN, DRILLUP, CHANGETOAUGER, AUGERDOWN, MELTING, AUGERUP
}STATE;

#define UINTMAX 4294967295
#define INT32MAX 2147483647
#define MAXLOAD 15
//33.72 is max load (150N) in lbs. 15 is a magic number. Giving some breathing room for motor response time. (33.72/2)-2 ~= 15
//ridiculously high number used for testing so thresh is never triggered

static uint32_t clock_freq = FREQ_24MHZ;
static STATE state;//keeps track of state for emergencies
static float WOBs[1000];//array of 1000 WOB readings to print at a time
static int32_t encoderVals[1000];//array of 1000 WOB readings to print at a time
static int32_t index;

void changeToDrill();
void changeToAuger();

void initState(void)
{
    set_DCO(clock_freq);
    initLoadCells();
    printf("Init Load Cells\n");
    initZMotorDrives();
    printf("Init Z-Motor Drives\n");
    initZMotorEncoders();
    printf("Init Z-Motor Encoders\n");
    initStepperMotor();
    printf("Init Stepper Motors\n");
    initLimitSwitches();
    printf("Init Limit Switches\n");
    initAugerRotMotor();
    printf("Init Auger Rotating Motor\n");
    initServoMotor(clock_freq);
    printf("Init Servo Motor\n");
    initThermocouple();
    printf("Init Thermocouple\n");
    initValves();
    printf("Init Valves\n");
    initHeaters();
    printf("Init Heaters\n");
    initPump();
    printf("Init Pump\n");
}

void emergencyState(void)
{//state for emergencies i.e. limit switches/frozen bit
    if (getError()) {//check limit switches. OTHER SOURCES OF ERROR???
        //return;//THIS IS JUST TO KILL THE STATE FOR TESTING PURPOSES.
        //************************************************************
        //NEED TO IMPLEMENT CORRECT EXIT FROM EMERGENCY STATE
        //************************************************************

        setDrillSpeed(0);//stops drill translation
        setAugerSpeed(0);//stops auger translation
        setAugerRotSpeed(0);//stops auger rotation
        killStepperMotor();//stops stepper motor translation
        drillOff();//stops drill rotation

        printf("EMERGENCY\n");//for some reason sometimes it just randomly goes into emergency state. Need to check if this is vibration on limit switch or software.
        //update 6/1: likely an issue with pin use. need to remap and try again with new understanding of pins

        //do something based on STATE
        switch (state) {
        case DRILLDOWN://hitting bottom z-axis drill limit switch requires translating up 31 inches to reset

            printf("Error in DRILLDOWN state\n");
            reverseServo(clock_freq);//reverse drill rotation direction
            //drillOn();//turn drill on
            setValDrill(224750);//encoder val corresponding to -31 inches
            setDrillSpeed(1000);//reverse out of hole
            delay_ms(1500, clock_freq);//give the plate a chance to step off the limit switch before reseting
            resetError();//reset limit switch
            while (getValDrill() > 0) {//waits until encoders read 0 or greater to stop spinning
                //unless another limit switch gets hit
                if (getError()) {
                    drillOff();
                    setDrillSpeed(0);
                    printf("TERMINATING PROGRAM");
                    delay_ms(500, clock_freq);
                    while (1);//block forever (program termination)
                }
            }
            setDrillSpeed(0);
            drillOff();
            setValDrill(206625);
            printf("Exiting EMERGENCY State...\n");
            break;

        case DRILLUP://hitting top z-axis drill limit switch requires translating down 1 inches to reset.

            printf("Error in DRILLUP state\n");
            setDrillSpeed(-1000);//send drill downward to reset position
            setValDrill(-7250);//represents 1 inches down
            delay_ms(1500, clock_freq);//give the plate a chance to step off the limit switch before reseting
            resetError();//reset limit switch
            while (getValDrill() < 0) {
                if (getError()) {//would not stop if attempting to go downward where there is no hole. SHOULD INCLUDE THIS POSSIBILITY, BUT BE CAUTIOUS TO
                    //NOT TERMINATE ENTIRE PROGRAM IN THE CASE OF ONE OR A FEW WLD DATA POINTS. GOAL IS TO NOT LET IT SMASH INTO THE GROUND
                    setDrillSpeed(0);
                    printf("TERMINATING PROGRAM");
                    while (1);//block forever (program termination)
                }
            }
            setDrillSpeed(0);
            printf("Exiting EMERGENCY State...\n");
            break;

        case CHANGETOAUGER://hitting right x-axis limit switch requires translating right __ inches to reset

            printf("Error in CHANGETOAUGER state\n");
            MoveRight(1500);//move the stepper to the right a certain distance
            delay_ms(1500, clock_freq);//give the plate a chance to step off the limit switch before reseting
            resetError();//reset limit switch
            changeToDrill();
            printf("Exiting EMERGENCY State...\n");
            break;

        case CHANGETODRILL://hitting left x-axis limit switch requires translating left __ inches to reset

            printf("Error in CHANGETODRILL state\n");
            MoveLeft(1500);//move the stepper to the left a certain distance
            delay_ms(1500, clock_freq);//give the plate a chance to step off the limit switch before reseting
            resetError();//reset limit switch
            changeToAuger();
            printf("Exiting EMERGENCY State...\n");
            break;

        case AUGERDOWN://hitting bottom z-axis auger limit switch requires translating up __ inches to reset

            printf("Error in AUGERDOWN state\n");
            setAugerSpeed(1000);//move the auger z-axis up a certain small distance
            state = AUGERUP;
            delay_ms(3000, clock_freq);//give some time to step off liit switch before sending all the way back to the surface
            resetError();
            while (1) {
                emergencyState();
            }

        case MELTING://WHAT MAY GO WRONG HERE???

            printf("Error in MELTING state\n");
            setAugerSpeed(1000);//move the auger z-axis up a certain small distance
            delay_ms(4000, clock_freq);//give the plate a chance to step off the limit switch before reseting
            setAugerSpeed(0);
            resetError();//reset limit switch
            printf("Exiting EMERGENCY State...\n");
            break;

        case AUGERUP://hitting top z-axis auger limit switch requires translating down __ inches to reset

            printf("Error in AUGERUP state\n");
            setAugerSpeed(-1000);//move the auger z-axis down a certain small distance TBD.
            delay_ms(3000, clock_freq);//give the plate a chance to step off the limit switch before reseting
            resetError();//reset limit switch
            changeToDrill();
            printf("TERMINATING PROGRAM\n");
            while(1);//TERMINATE PROGRAM

        }
        resetError();//reset limit switches to not doublecount
    }
}

void printWOBs() {
    int i;

    for (i = 0; i < index; i++) {
        printf("WOB[%d]: %.2f, Enc: %d\n", i, WOBs[i], encoderVals[i]);
    }
    index = 0;//reset index

}

void drillDownState(void)
{//drill downward until reaching desired encoder value
    state = DRILLDOWN;
    printf("Drill Down State...\n");
    float WOB;
    int32_t val = 0;
    index = 0;

    do {//check encoders to verify downward movement

        val = getValDrill();//get encoder val
        WOB = readLoadCells();//get load cell val

        if (WOB >= MAXLOAD) {//set limit for WOB before slowing drill descent
            setDrillSpeed(0);//max load reached, stop downward rot
            drillOn();//turn on drill in case it didn't turn on already by the time it reached the overburden
            //WARNING!!!!!!!!!!! This will cause the drill to spin if you apply weight to the bit in order to test it
        }
        else {
            setDrillSpeed(-1000);//load is less than MAXLOAD, so drill max downward
        }

        //Send data to cache
        //printf("%.2f %d\n", WOB, val);
        WOBs[index] = WOB;
        encoderVals[index] = val;
        index++;

        if (index == 1000) {//cache reached max
            setDrillSpeed(0);//stop vert motion
            printWOBs();//print values
        }

        if (val > 18125) {//turn drill on after 2.5 in
            drillOn();
            //drillOff();
        }

        emergencyState();//a good idea to get in practice of running alongside drill movement to check limit switches

    } while (val < 206625);//28.5 in is 206625. 15 in is 108750
    setDrillSpeed(0);
    printWOBs();
    //drill stops and state breaks when encoder value is reached
}

void drillUpState(void)
{//drill upwards until reseting position
    state = DRILLUP;
    printf("Drill Up State...\n");
    int32_t val = 0;
    float WOB;

    setDrillSpeed(0);//stop it translating just in case
    drillOff();//stop it spinning
    reverseServo(clock_freq);//reverse servo

    do {//check encoders to see if position has been returned to zero

        val = getValDrill();
        WOB = readLoadCells();

        if (WOB < -1 * MAXLOAD) {//edge case where drill bit is stuck while trying to get out
            drillOn();
        }
        else {//if smooth travels
            drillOff();
        }

        setDrillSpeed(1000);//max upward
        emergencyState();
        //printf("%.2f %d\n", WOB, val);

    } while (val > 0);

    setDrillSpeed(0);//stop drill motion
    drillOff();//just in case
    //NEED TO DEBUG THIS. ONLY WORKS WHEN PRINT STATEMENT ADDED. OBV WILL MESS UP ENCODER VALS THAT WAY.
    // ^^^ Not sure when I wrote this or if it's still true
}

void changeToDrill()
{//change the tool state right to drill
    state = CHANGETODRILL;
    printf("Change to Drill State...\n");
    int32_t countdown = 310;

    MoveRight(31000);//move left for 30 seconds = 7.5 inches

    while (countdown > 0) {
        emergencyState();
        countdown--;
        delay_ms(100, clock_freq);
    }
}

void changeToAuger()
{//change the tool state left to auger
    state = CHANGETOAUGER;
    printf("Change to Auger State...\n");
    int32_t countdown = 310;

    MoveLeft(31000);//move left for 30 seconds = 7.5 inches

    while (countdown > 0) {
        emergencyState();//check this 10 times a second
        countdown--;
        delay_ms(100, clock_freq);//delay for 0.1 seconds
    }
}

void augerDownState(void)
{//this will send the auger into the hole excavated of dirt
    state = AUGERDOWN;
    printf("Auger Down State...\n");
    int32_t val = 0;

    do {
        val = getValAug();

        //can use current sensors here to modulate the speed of descent if the overburden is more difficult to twist through in some areas
        setAugerSpeed(-1000);//otherwise full send till we hit the bottom    OBVIOUSLY NOT HOW IT WILL BE IMPLEMENTED PERMANENTLY**********

        emergencyState();

        if (val > 47125) {//6.5 inches for auger to rotate. should go clockwise from aerial view
            setAugerRotSpeed(-1000);//negative is clockwise from aerial view
        }

    } while (val < 203000);//number to match drill (slightly higher up tho to avoid impact with ice). 28 in is 203000. 14in is 101500
    setAugerSpeed(0);
    setAugerRotSpeed(0);
    //when encoder reached, stop auger and break state
}

void meltingState(void)
{
    int setTemp = 300;//set how hot to let the heater probe get before turning it off and letting it cool back down
    //uint8_t done = 0;//Flag that tells our system when the whole thing is done
    //uint8_t HPCount = 0;//variable that records how many times the heater Probe has turned
    uint8_t rotationCount = 0;//Variable that counts the circulation cycles that we've gone through
    uint8_t maxRotations = 45;//Variable that stores the upper limit of total cycles we intend to go through at a particular depth
    uint8_t cycleCount = 0;//Variable that counts the total sets of circulation cycles that we've gone through
    uint8_t maxCycles = 10;//Variable that stores the upper limit of sets of total cycles we intend to go through

    state = MELTING;
    printf("Melting State...\n");

    //Bring the heater to a hot temp and initiate initial melt
    //initialMelt();

    for (cycleCount = 0; cycleCount < maxCycles; cycleCount++) {//this for loop is run every time a change in depth occurs

        //Start with a small change of depth
        setAugerSpeed(-1000);
        delay_ms(3000, clock_freq);
        setAugerSpeed(0);
        emergencyState();

        //Then do a filter dump
        filterDump(1200);//does this for 5 sec. Can change in melting.c. Want to do short to not waste water, but enough to get water through tubes

        for (rotationCount = 0; rotationCount < maxRotations; rotationCount++) {//this for loop is run every time a rotation occurs

            //Start with a small rotation
            setAugerRotSpeed(-1000);
            delay_ms(30, clock_freq);//this can change the amount of rotation, but keeping it small is best
            setAugerRotSpeed(0);

            //Check temperature
            if (readTemp() > setTemp) {//If temperature of the cartridge heater too high
              Heater1Off();   //Turn off the cartridge heater
            }
            else {//If temperature of the cartridge heater isn't too high
              Heater1On();  //Turn on the cartridge heater
            }
            //HEATER2 NEVER IMPLEMENTED
            //Heater2Off();//reset heater 2 since it never gets turned off otherwise

            //Melt and Extract
            //extraction(5000);

            //Recirculate and Water Jet
            waterJet(10000);
        }
        //Extract some
        extraction(15000);
    }

    //When the loops end, it will have completed a water jetting. Now do a final succ to get that good shit
    extraction(10000);
    delay_ms(15000, clock_freq);//pause in between to let micro filter catch up a bit
    extraction(10000);
    delay_ms(15000, clock_freq);
    extraction(30000);

    //And turn things off
    PumpOff();
    Heater1Off();
}

void augerUpState(void)
{//extract the auger back out of the overburden
    state = AUGERUP;
    printf("Auger Up State...\n");

    int32_t val = 0;

    do {//reverse out of the hole after completion
        val = getValAug();

        //setAugerRotSpeed(-1000);
        setAugerSpeed(1000);

        emergencyState();

    } while (val > 0);
    setAugerSpeed(0);
    //when encoder reached, stop auger and break state
}

void test()
{
    Valve2Open();
    PumpForward();
    //setAugerSpeed(1000);
    //delay_ms(1200, clock_freq);
    //setAugerRotSpeed(1000);
    //changeToAuger();
    //setDrillSpeed(1000);
    //drillDownState();

    //setValDrill(206645);
    //drillUpState();//drill up and extract tool

    //changeToAuger();//change the auger tool

    //augerDownState();//send auger down into drilled hole.

    //meltingState();//melt and extract the water

    //augerUpState();//extract the auger tool and terminate the program

    //changeToDrill();//reset drill position for
    //meltingState();

    /*
    int32_t val;
    printf("Initiating test...\n");

    state = AUGERDOWN;
    do {
        val = getValAug();
        setAugerSpeed(-1000);
        emergencyState();
    } while (val < 10000);
    setAugerSpeed(0);
    delay_ms(6000, clock_freq);//stop for measurement
    state = AUGERUP;
    do {
        val = getValAug();
        setAugerSpeed(1000);
        emergencyState();
    } while (val > 0);
    setAugerSpeed(0);
    printf("DONE\n");
    //setDrillSpeed(-1000);
    //drillOn();

    printf("DONE\n");
    */

    while (1) {

        //setDrillSpeed(1000);
        //;
        //printf("%.2f\n", getVal());
        //emergencyState();
        //readLoadCells();
        //delay_ms(100, clock_freq);

    }
}

void reset() {
    //setAugerRotSpeed(-1000);//clockwise from aerial view
    //setAugerSpeed(1000);
    //setAugerSpeed(-1000);
    //setDrillSpeed(1000);
    //setDrillSpeed(-1000);
    //changeToAuger();//
    //changeToDrill();
    while (1) {
        ;
    }
}

void main(void)
{
    //drill down is negative
    //auger down is negative
    //auger clockwise (aerial) is negative

    initState();//initialize everything

    //reset();
    //test();

    //Move the drill to be over the hole at the start of the test so we don't accidently try to reset the position with the auger not fully extracted
    //at the end of the test
    changeToDrill();//move drill back to original position (if it has been moved already)
    //changeToAuger();//goes the other way

    drillDownState();//drill downward and carve out a hole

    drillUpState();//drill up and extract tool

    changeToAuger();//change the auger tool

    augerDownState();//send auger down into drilled hole.

    meltingState();//melt and extract the water

    augerUpState();//extract the auger tool and terminate the program

    changeToDrill();//reset drill position for

    printf("DONE!!! HURRAY!!\n");

    while (1);//block forever (program termination)
}

//To test how many encoder readings are lost in 100,000 runs of readLoadCells();
    //      FOUND TO BE ABOUT 1 inch in every 30 inches. Or 250/7500 per inch. Not a concern.

    //Also Testing if can successfully re-zero after the test by reversing until encoder reads 0.
    //      FOUND TO BE A BIT UNDER 1/2 inch not long enough going from 55505->0. Or 1/2inch per 7.5 inches. To go down 30 inches would be off by 2 inches. More of an issue.
    // For instance:
    // If we set machine to run until encoder reads 225 000. It will go about 31 inches instead of 30. Also it will go up 29 instead of 30, therefore leaving it 2 inches lower than it started!!
