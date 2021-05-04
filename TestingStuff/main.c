#include "msp.h"
#include "motorDriver.h"
#include "delay.h"
#include "limitSwitch.h"
#include "encoder.h"
#include "LoadCellDataCollection.h"
#include <stdio.h>
#include "stepperMotor.h"

/**
 * main.c
 */

#define UINTMAX 4294967295

void main(void)
{
    int32_t dir;

    uint32_t clock_freq = FREQ_24MHZ;
    set_DCO(clock_freq);
    initLoadCells();
    initMotorDrives();
    initMotorEncoders();
    initStepperMotor();
    initLimitSwitches();

    uint32_t thresholdDown = 25;   //lbs
    uint32_t thresholdUp = 20;     //lbs

    //printf("Direction? (up: 1, down: 2)\n");
    //scanf("%d", &dir);

    dir = 2;

    //****************
    while (1) {
        if (dir == 1) {
            setSpeed(-1000);  //100% down
            //delay_ms(10000, clock_freq);   //10s
            while (1) {
                printf("Drill Encoder: %d\n", getValDrill());
            }
        }
        else if (dir == 2) {
            setSpeed(1000);   //100% up
            delay_ms(10000, clock_freq);    //10s
        }
        //readLoadCells(thresholdDown, DOWN);
        setSpeed(0);
        //delay_ms(UINTMAX, clock_freq);
        //readLoadCells(thresholdUp, UP);
    }
    //****************

}
