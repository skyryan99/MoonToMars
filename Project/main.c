#include "msp.h"
#include "motorDriver.h"
#include "delay.h"
#include "limitSwitch.h"
#include "encoder.h"
#include "LoadCellDataCollection.h"
#include <stdio.h>
#include "stepperMotor.h"
#include "augerRot.h"

/**
 * main.c
 */

/*
 * Some info:
 * -clock_freq can be set at various values, all found in "delay.h". This variable controls how fast the microcontroller runs instructions
 * -z-axis directions can change depending on order of pins, order of wire connections, or code. This is kept track of below for clarity
 * -encoders: 7500 ticks corresponds to 1 inch. Maximum vertical range of 30 in = 225 000 ticks.
 */

#define UINTMAX 4294967295
#define INT32MAX 2147483647

static uint8_t emergency;//0: no emergency, 1: in emergency state, not yet addressed
static uint8_t maxLoad;//set to for control loop

void initState(void)
{
    uint32_t clock_freq = FREQ_24MHZ;
    set_DCO(clock_freq);
    initLoadCells();
    initZMotorDrives();
    initZMotorEncoders();
    initStepperMotor();
    initLimitSwitches();
    initAugerRotMotor();
}

void emergencyState(void)
{
    if (emergency) {
        //do something
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
void main(void)
{
    //drill down is positive
    //auger down is positive

    initState();

    //****************

    while (1) {

        /*Check emergency conditions*/
        emergencyState();

        /*Move the stepper motor - time is in ms*/
        MoveRight(12000);

        printf("Done!\n");
        break;
    }
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
