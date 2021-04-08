#include "msp.h"
#include <stdio.h>
#include "motorDrive.h"
#include "delay.h"
#include "LoadCellDataCollection.h"

/**
 * main.c
 */

void main(void)
{
    /*
     * CW is UP
     * CCW is DOWN
     *
     */

    uint32_t clock_freq = FREQ_24MHZ;
    set_DCO(clock_freq);
    initMotorDrive();
    float thresholdDown = 30; //lbs
    float thresholdUp = 5; //lbs

    //****************
    while (1) {
        setSpeed(1000);                //100% down
        delay_ms(5000000, clock_freq);      //5000s
        readLoadCells(thresholdDown, 1);
        setSpeed(0);
        readLoadCells(thresholdUp, 2);
    }

    delay_ms(500000, clock_freq);      //500s
    //****************



    //*****************
    setSpeed(-1000);                //100% down
    delay_ms(500, clock_freq);      //0.5s

    setSpeed(0);
    readLoadCells(thresholdDown, 1);
    printf("Done!");
    delay_ms(9000000, clock_freq);    //9000s

    //*****************

    setSpeed(-1000);                //100% down
    //blocks until WOB exceeds threshold
    readLoadCells(thresholdDown, 1);

    //delay_ms(90000, clock_freq);    //90s

    setSpeed(0);                    //off
    delay_ms(30000, clock_freq);    //5s

    setSpeed(1000);                 //100% up
    delay_ms(90000, clock_freq);    //90s

    setSpeed(0);                    //off
}
