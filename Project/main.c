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
 * -z-axis directions can change depending on order of pins, order of wire connections, or code. This is kept track of in main() for clarity
 * -encoders: 7500 ticks corresponds to 1  without any blocking. Maximum vertical range of 30 in = 225 000 ticks. Test for true values
 * -stepper motor at 1800 period runs at 0.25 inches / sec. To move 7.5in -> run for 30 seconds. Quite precise, can rely on timing for distance
 * -readLoadCells(), forwardServo(), reverseServo(), emergencyState(), and all STATE functions block for a some amount of time
 */

typedef enum {
    CHANGETODRILL, DRILLDOWN, DRILLUP, CHANGETOAUGER, AUGERDOWN, MELTING, AUGERUP
}STATE;

#define UINTMAX 4294967295
#define INT32MAX 2147483647
#define MAXLOAD 13
//33.72 is max load (150N) in lbs. 18/16 is a magic number. Giving some breathing room for motor response time

static uint32_t clock_freq = FREQ_24MHZ;
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
    initServoMotor(clock_freq);
    //printf("Init Servo Motor\n");

    //INIT STUFF FROM HEATER, VALVES, THERMOCOUPLE, PUMP

    //Move the drill to be over the hole at the start of the test so we don't accidently try to reset the position with the auger not fully extracted
    //at the end of the test
    //changeToDrill();
}

void emergencyState(void)
{//state for emergencies i.e. limit switches/frozen bit
    if (getError()) {//check limit switches. OTHER SOURCES OF ERROR???

        setDrillSpeed(0);//stops drill translation
        setAugerSpeed(0);//stops auger translation
        setAugerRotSpeed(0);//stops auger rotation
        killStepperMotor();//stops stepper motor translation
        drillOff();//stops drill rotation

        printf("EMERGENCY\n");//for some reason sometimes it just randomly goes into emergency state. Need to check if this is vibration on limit switch or software.
        //update 6/1: likely an issue with pin use. need to remap and try again with new understanding of pins

        //do something based on STATE
        switch (state) {
        case DRILLDOWN://hitting bottom z-axis drill limit switch requires translating up 32 inches to reset

            printf("Error in DRILLDOWN state\n");
            reverseServo(clock_freq);//reverse drill rotation direction
            drillOn();//turn drill on
            setValDrill(232000);//encoder val corresponding to -32 inches     CHANGE VALUE LATER**************
            setDrillSpeed(-1000);//reverse out of hole
            delay_ms(500, clock_freq);//give the plate a chance to step off the limit switch before reseting
            resetError();//reset limit switch
            while (getValDrill() > 0) {//waits until encoders read 0 or greater to stop spinning
                //unless another limit switch gets hit
                if (getError()) {
                    drillOff();
                    setDrillSpeed(0);
                    while (1);//block forever (program termination)
                }
            }
            setDrillSpeed(0);
            drillOff();
            break;

        case DRILLUP://hitting top z-axis drill limit switch requires translating down 20 inches to reset

            printf("Error in DRILLUP state\n");
            setDrillSpeed(1000);//send drill downward to reset position     CHANGE VALUE LATER**************
            setValDrill(-145000);//represents 20 inches down
            while (getValDrill() < 0) {
                if (getError()) {//would not stop if attempting to go downward where there is no hole. SHOULD INCLUDE THIS POSSIBILITY, BUT BE CAUTIOUS TO
                    //NOT TERMINATE ENTIRE PROGRAM IN THE CASE OF ONE OR A FEW WLD DATA POINTS. GOAL IS TO NOT LET IT SMASH INTO THE GROUND
                    setDrillSpeed(0);
                    while (1);//block forever (program termination)
                }
            }
            setDrillSpeed(0);
            break;

        case CHANGETOAUGER://hitting right x-axis limit switch requires translating right __ inches to reset

            printf("Error in CHANGETOAUGER state\n");
            //move the stepper to the left a certain distance, TBD
            break;

        case CHANGETODRILL://hitting left x-axis limit switch requires translating left __ inches to reset

            printf("Error in CHANGETODRILL state\n");
            //move the stepper to the right a certain distance, TBD
            break;

        case AUGERDOWN://hitting bottom z-axis auger limit switch requires translating up __ inches to reset

            printf("Error in AUGERDOWN state\n");
            //move the auger z-axis up a certain small distance TBD.
            break;

        case MELTING://WHAT MAY GO WRONG HERE???

            printf("Error in MELTING state\n");
            break;

        case AUGERUP://hitting top z-axis auger limit switch requires translating down __ inches to reset

            printf("Error in AUGERUP state\n");
            //move the auger z-axis down a certain small distance TBD.
            break;

        }
        resetError();//reset limit switches to not doublecount
    }
}

void drillDownState(void)
{//drill downward until reaching desired encoder value
    state = DRILLDOWN;
    float WOB;
    int32_t val = 0;

    do {//check encoders to verify downward movement. 7500 ticks = 1in. WHEN PRINTING DATA, NEW CONVERSION USED. ~35.85 ticks = 1 in.

        val = getValDrill();//get encoder val
        WOB = readLoadCells();//get load cell val

        if (WOB >= MAXLOAD) {//set limit for WOB before slowing drill descent
            setDrillSpeed(0);//max load reached, stop downward rot
            //drillOn();//turn on drill in case it didn't turn on already.
            //WARNING!!!!!!!!!!! This will cause the drill to spin if you apply weight to the bit in order to test it
        }
        else {
            setDrillSpeed(-1000);//load is less than MAXLOAD, so drill max downward
        }
        //printf("%.2f %d\n", WOB, val);

        if (val > 200) {//turn drill on after 200 encoders     CHANGE VALUE LATER***************
            drillOn();
        }

        emergencyState();//a good idea to get in practice of running alongside drill movement to check limit switches

    } while (val < 620);//620 magic number (used when print statements used)       CHANGE VALUE LATER************
    setDrillSpeed(0);
    //drill stops and state breaks when encoder value is reached
}

void drillUpState(void)
{//drill upwards until reseting position
    state = DRILLUP;
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

        setDrillSpeed(-1000);//max upward
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
    int32_t countdown = 300;

    MoveRight(30000);//move left for 30 seconds = 7.5 inches

    while (countdown > 0) {
        emergencyState();
        countdown--;
        delay_ms(100, clock_freq);
    }

    //THIS NEEDS TO BE TESTED TO MAKE SURE THE EXTRA INSTRUCTIONS DON'T MESS WITH THE DELAY TIMING.
}

void changeToAuger()
{//change the tool state left to auger
    state = CHANGETOAUGER;
    int32_t countdown = 300;

    MoveLeft(30000);//move left for 30 seconds = 7.5 inches

    while (countdown > 0) {
        emergencyState();//check this 10 times a second
        countdown--;
        delay_ms(100, clock_freq);//delay for 0.1 seconds
    }

    //THIS NEEDS TO BE TESTED TO MAKE SURE THE EXTRA INSTRUCTIONS DON'T MESS WITH THE DELAY TIMING.
}

void augerDownState(void)
{//this will send the auger into the hole excavated of dirt
    state = AUGERDOWN;
    int32_t val = 0;

    //can use current sensors here to modulate the speed of descent if the overburden is more difficult to twist through in some areas
    setAugerSpeed(1000);//otherwise full send till we hit the bottom    OBVIOUSLY NOT HOW IT WILL BE IMPLEMENTED PERMANENTLY**********
    setAugerRotSpeed(1000);

    do {
        val = getValAug();
        emergencyState();
    } while (val < 620);//magic number to match drill             CHANGE VALUE LATER*************
    setAugerSpeed(0);
    setAugerRotSpeed(0);
    //when encoder reached, stop auger and break state
}

void meltingState(void)
{//this will be jacob's finite state machine
    state = MELTING;

    //Initital code to be run when the probe is in the ground
    int HPCount;        //variable that records how many times the heater Probe has turned
    int Done;           //Flag that tells our system when the whole thing is done
    int AmountOfCycles; //Variable that stores the upper limit of total cycles we intend to go through
    int CycleCount;     //Variable that counts the circulation cycles that we've gone through (water jet, recirculation to clear the suction hole, sucking out water)
    int lion;           //Variable used for a "delay" function when recirculation water for the heater probe
    int Temp;           //Variable used to store the data from the temperature sensor to determine how hot our cartridge heater is
    int settemp;
    Heater1On();       //Start by turning the heater on to melt the ice
    PumpForward();     //Start pumping, can run dry so it isn't a problem
    Valve2Close();     //Close the outlet valve to begin with, just want it to circulate water in the water jet
    Valve3Open();      //Open the water jet valve
    Valve1Close();     //Close the valve that we use to dump out the system
    HPCount = 0;       //Reset the heater probe counter
    Done = 0;          //Start by making sure the system doesn't thing we're done
    AmountOfCycles = 4;//Change this to however many cycles we want to use
    CycleCount = 0;    //To begin with, we haven't gone through any cycles
    initThermocouple();
    settemp = 50;


    while(1)
    {
//        int cat;                          //Code for testing, to be replaced by recording turns of the motor in the interrupt that turns the motor
//        for (cat=0; cat < 500000; cat++)  //
//        {                                 //
//        }                                 //
//        HPCount = HPCount + 1;            //

      //Temperature module, must be run through first each time for safety (module refered to as "State 1")
      Temp = readvalue(temp reading pin); //Not the actual code for reading the thermocouple, just an assumption of the code and placeholder

        Temp = MAX31855ReadTemperature(); //<-- Rebecca said this should work and I trust her
      if (Temp > settemp) //If temperature of the cartridge heater too high
      {
          Heater1Off();   //Turn off the cartridge heater
      }
      else //If temperature of the cartridge heater isn't too high
      {
          Heater1On();  //Turn on the cartridge heater
      }

        if (HPCount < 3 && CycleCount < AmountOfCycles && Done == 0) //State 2 Water jet function
        {
            Valve2Close();
            Valve3Open();
            Heater2On();
        }
        else if (HPCount == 3 && CycleCount < AmountOfCycles && Done == 0) //recirc state. Base coming here on doing so a certain number of times
            //when implemented in actual code. This should be done for a few minutes and then skipped to give time to sucking out water.
        {
            Valve2Close();
            Valve3Open();
            Heater2Off();
            PumpBackward();
            for (lion = 0; lion < 100000; lion++)
            {
            }
            HPCount = HPCount + 1;
        }
        else if (HPCount == 4 && CycleCount < AmountOfCycles && Done == 0) //State 3 Extract Water
        {
            PumpForward();
            Valve2Open();
            Valve3Close();
            Heater2Off();
        }
        else if (HPCount > 4 && HPCount < 7 && CycleCount < AmountOfCycles && Done == 0) //State 4 Water jet function
        {
            Valve2Close();
            Valve3Open();
            Heater2On();
        }
        else if (HPCount == 7 && CycleCount < AmountOfCycles && Done == 0) //Recirc state
        {
            Valve2Close();
            Valve3Open();
            Heater2Off();
            PumpBackward();
            for (lion = 0; lion < 100000; lion++)
            {
            }
            HPCount = HPCount + 1;
        }
        else if (HPCount == 8 && CycleCount < AmountOfCycles && Done == 0) //State 5 Extract Water
        {
            PumpForward();
            Valve2Open();
            Valve3Close();
            Heater2Off();
            HPCount = 0;
            CycleCount += 1;
        }
        else if (HPCount == 1 && CycleCount >= AmountOfCycles && Done == 0) //Recirc state
        {
            Valve2Close();
            Valve3Open();
            Heater2Off();
            PumpBackward();
            for (lion = 0; lion < 100000; lion++)
            {
            }
            HPCount = HPCount + 1;
        }
        else if (HPCount < 5 && CycleCount >= AmountOfCycles && Done == 0) //State 6 Final suction state, Can adjust HPCount to make it longer or shorter
        {
            PumpForward();
            Valve2Open();
            Valve3Close();
            Heater2Off();
        }
        else if (HPCount >= 5 && CycleCount >= AmountOfCycles && Done == 0) //State 7 clear out system
        {
            PumpBackward();
            Valve2Close();
            Valve3Close();
            Valve1Open();
            Heater1Off();
            Done = 1;
        }
        else if (Done == 1) //State 8 break free from this cruel world
        {
            break;
        }
    }
}

void augerUpState(void)
{//extract the auger back out of the overburden
    state = AUGERUP;
    int32_t val = 0;

    setAugerSpeed(-1000);

    do {//reverse out of the hole after completion
        val = getValAug();
        emergencyState();
    } while (val < 0);
    setAugerSpeed(0);
    //when encoder reached, stop auger and break state
}

void main(void)
{
    //drill down is positive
    //auger down is positive

    initState();//initialize everything

    drillDownState();//drill downward and carve out a hole

    drillUpState();//drill up and extract tool

    changeToAuger();//change the auger tool

    augerDownState();//send auger down into drilled hole.

    meltingState();//melt and extract the water

    augerUpState();//extract the auger tool and terminate the program

    while (1);//block forever (program termination)
}

//To test how many encoder readings are lost in 100,000 runs of readLoadCells();
    //      FOUND TO BE ABOUT 1 inch in every 30 inches. Or 250/7500 per inch. Not a concern.

    //Also Testing if can successfully re-zero after the test by reversing until encoder reads 0.
    //      FOUND TO BE A BIT UNDER 1/2 inch not long enough going from 55505->0. Or 1/2inch per 7.5 inches. To go down 30 inches would be off by 2 inches. More of an issue.
    // For instance:
    // If we set machine to run until encoder reads 225 000. It will go about 31 inches instead of 30. Also it will go up 29 instead of 30, therefore leaving it 2 inches lower than it started!!
