#include "msp.h"
#include "LoadCellDataCollection.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * 1. Provide 25 clock input signals to set input channel A to a gain of 128
 * 2. Initiate data collection by sending SDA low
 * 3. Transfer bits
 */

static float WOB;//keep track of last data point to filter random jumps
static unsigned long OFFSET_L;//offset for Left/Green Load Cell
//static unsigned long OFFSET_R;//offset for Right/Red Load Cell

void initLoadCells()
{
    /*Load Cell 1*/
    /* Input - DT*/
    DATA_PIN_L->SEL1 &= ~(DATA_BIT_L);                   /* Use GPIO */
    DATA_PIN_L->SEL0 &= ~(DATA_BIT_L);
    DATA_PIN_L->DIR &= ~(DATA_BIT_L);                    /*0 - input*/
    //DATA_PIN_L->REN |= DATA_BIT_L;                       /*enable pull resistor*/
    //DATA_PIN_L->OUT |= DATA_BIT_L;                       /*select pull UP resistor*/

    /* Output - SCK*/
    CLK_PIN_L->SEL1 &= ~(CLK_BIT_L);                     /* Use GPIO */
    CLK_PIN_L->SEL0 &= ~(CLK_BIT_L);
    CLK_PIN_L->DIR |= CLK_BIT_L;                         /*1 - output*/

    //DATA_PIN_L->OUT |= DATA_BIT_L;                         /*set DT high*/ //THIS IS JUST EVALUATING AN EXPRESSION, NOT SETTING A VALUE. REVISIT LATER.....
    CLK_PIN_L->OUT &= ~(CLK_BIT_L);                      /*set SCK low */

    /*Load Cell 2*/
    /* Input - DT*/
    DATA_PIN_R->SEL1 &= ~(DATA_BIT_R);                    /* Use GPIO */
    DATA_PIN_R->SEL0 &= ~(DATA_BIT_R);
    DATA_PIN_R->DIR &= ~(DATA_BIT_R);                     /*0 - input*/
    //DATA_PIN_R->REN |= DATA_BIT_R;                        /*enable pull resistor*/
    //DATA_PIN_R->OUT |= DATA_BIT_R;                        /*select pull UP resistor*/

    /* Output - SCK*/
    CLK_PIN_R->SEL1 &= ~(CLK_BIT_R);                      /* Use GPIO */
    CLK_PIN_R->SEL0 &= ~(CLK_BIT_R);
    CLK_PIN_R->DIR |= CLK_BIT_R;                          /*1 - output*/

    //DATA_PIN_R->OUT |= DATA_BIT_R;                          /*set DT high*/ //SAME HERE >.........................
    CLK_PIN_R->OUT &= ~(CLK_BIT_R);                       /*set SCK low */

    initOffsets();
    WOB = 0;//magic number
}

void initOffsets() {
    uint8_t i = 0;
    float sum = 0, temp;

    //Flush one reading
    readCount1();
    readCount2();

    while (i < 20) {
        temp = readCount1();
        //printf("%f\n", temp);
        if (temp < 8100000 && temp > 6900000) {//filter bad data
            sum += temp;
            i++;
        }
    }
    sum = sum / 20;//get avg
    OFFSET_L = sum;
    /*LOAD CELL NOT WORKING
    sum = 0;//now do other load cell
    while (1 < 20) {
        temp = readCount2();
        if (temp < 8500000) {
            sum += temp;
            i++;
        }
    }
    sum = sum / 20;//get avg
    OFFSET_R = sum;
    */
}

float readLoadCells()
{
    float val1, temp, coef = 0.75;
    //float val2;//LOAD CALL NOT WORKING

    /*Collect readings*/
    val1 = (float)readCount1();//setting this to OFFSET_L makes the reading useless and yield 0, for testing purposes
    //val2 = (float)readCount2();//setting this to OFFSET_R makes the reading useless and yield 0, for testing purposes
    //LOAD CELL IS NOT WORKING

    /*Calculate WOB*/
    //temp = ((val1 - OFFSET_L) + (val2 - OFFSET_R)) / CALFACTOR;//both vals should have approx half the total weight. add for total
    temp = (val1 - OFFSET_L) / CALFACTOR;

    /*Prefilter bad data using last collected point*/
    if (abs(temp) > 2 * (abs(WOB) + 2)) {
        WOB += 2;
    }
    else {
        WOB = coef * WOB + temp * (1 - coef);//low pass filter
    }

    //printf("WOB: %.2f\n", WOB);

    return WOB;
}

unsigned long readCount1(void)
{
    unsigned long count = 0;
    uint32_t i;

    while (DATA_PIN_L->IN & DATA_BIT_L);                    /*stay in this loop if DT high*/

    for (i = 0; i < 24; i++) {
        CLK_PIN_L->OUT |= CLK_BIT_L;                      /*set SCLK high to initialize data collection*/
        count = count << 1;
        CLK_PIN_L->OUT &= ~(CLK_BIT_L);                   /*set SCLK low to toggle clock*/
        if (DATA_PIN_L->IN & DATA_BIT_L)
            count++;
    }

    CLK_PIN_L->OUT |= CLK_BIT_L;                          /*set SCLK high to toggle clock*/
    count = count^0x800000;                               /* Read the MSB */
    CLK_PIN_L->OUT &= ~(CLK_BIT_L);                       /*set SCLK low to toggle clock*/

    return count;
}

// LOAD CELL IS NOT WORKING
unsigned long readCount2(void)
{
    unsigned long count = 0;
    uint32_t i;

    while (DATA_PIN_R->IN & DATA_BIT_R);                   //stay in this loop if DT high

    for (i = 0; i < 24; i++) {
        CLK_PIN_R->OUT |= CLK_BIT_R;                      //set SCLK high to initialize data collection
        count = count << 1;
        CLK_PIN_R->OUT &= ~(CLK_BIT_R);                   //set SCLK low to toggle clock
        if (DATA_PIN_R->IN & DATA_BIT_R)
            count++;
    }

    CLK_PIN_R->OUT |= CLK_BIT_R;                          //set SCLK high to toggle clock
    count = count^0x800000;                             // Read the MSB
    CLK_PIN_R->OUT &= ~(CLK_BIT_R);                       //set SCLK low to toggle clock
    return count;
}
