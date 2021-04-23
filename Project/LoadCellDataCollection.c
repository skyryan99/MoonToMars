#include "msp.h"
#include "LoadCellDataCollection.h"
#include <stdio.h>

/**
 * 1. Provide 25 clock input signals to set input channel A to a gain of 128
 * 2. Initiate data collection by sending SDA low
 * 3. Transfer bits
 */

void initLoadCells()
{
    /*Load Cell 1*/
    /* Input - DT - on P1.5*/
    DATA_PIN_L->SEL1 &= ~(DATA_BIT_L);                   /* Use GPIO */
    DATA_PIN_L->SEL0 &= ~(DATA_BIT_L);
    DATA_PIN_L->DIR &= ~(DATA_BIT_L);                    /*0 - input*/
    DATA_PIN_L->REN |= DATA_BIT_L;                       /*enable pull resistor*/
    DATA_PIN_L->OUT |= DATA_BIT_L;                       /*select pull UP resistor*/

    /* Output - SCK - on P6.1*/
    CLK_PIN_L->SEL1 &= ~(CLK_BIT_L);                     /* Use GPIO */
    CLK_PIN_L->SEL0 &= ~(CLK_BIT_L);
    CLK_PIN_L->DIR |= CLK_BIT_L;                         /*1 - output*/

    DATA_PIN_L->IN | DATA_BIT_L;                         /*set DT high*/
    CLK_PIN_L->OUT &= ~(CLK_BIT_L);                      /*set SCK low */

    /*Load Cell 2*/
    /* Input - DT - on P4.3*/
    DATA_PIN_R->SEL1 &= ~(DATA_BIT_R);                    /* Use GPIO */
    DATA_PIN_R->SEL0 &= ~(DATA_BIT_R);
    DATA_PIN_R->DIR &= ~(DATA_BIT_R);                     /*0 - input*/
    DATA_PIN_R->REN |= DATA_BIT_R;                        /*enable pull resistor*/
    DATA_PIN_R->OUT |= DATA_BIT_R;                        /*select pull UP resistor*/

    /* Output - SCK - on P6.4*/
    CLK_PIN_R->SEL1 &= ~(CLK_BIT_R);                      /* Use GPIO */
    CLK_PIN_R->SEL0 &= ~(CLK_BIT_R);
    CLK_PIN_R->DIR |= CLK_BIT_R;                          /*1 - output*/

    DATA_PIN_R->IN | DATA_BIT_R;                          /*set DT high*/
    CLK_PIN_R->OUT &= ~(CLK_BIT_R);                       /*set SCK low */
}

void readLoadCells(uint32_t threshold, uint8_t run)
{
    unsigned long val1, val2;
    float WOB = 0;

    while (1) {

        /*Collect readings*/
        val1 = readCount1();
        val2 = readCount2();

        /*Calculate WOB*/
        WOB = (((val1 - OFFSET_L) / (CAL_COUNT_L * CAL_WEIGHT)) + ((val2 - OFFSET_R) / (CAL_COUNT_R * CAL_WEIGHT))) * GAIN + BIAS; // Add both load cell values

        /*Print data to screen*/
        printf("WOB [N]: %.2f \n\r",WOB);

        /*Break when WOB exceeds threshold*/
        if (WOB > threshold && run == DOWN)
            return;
        /*Break as soon as WOB returns under threshold*/
        else if (WOB < threshold && run == UP)
            return;
    }
}

unsigned long readCount1(void)
{
    unsigned long count = 0;
    uint32_t i;

    //if !(DATA_PIN_L->IN & DATA_BIT_L)                    /*stay in this loop if DT high*/
    //    return 0;

    for (i = 0; i < 24; i++) {
        CLK_PIN_L->OUT |= CLK_BIT_L;                      /*set SCLK high to initialize data collection*/
        count <<= 1;
        CLK_PIN_L->OUT &= ~(CLK_BIT_L);                   /*set SCLK low to toggle clock*/
        if (DATA_PIN_L->IN & DATA_BIT_L)
            count++;
    }

    CLK_PIN_L->OUT |= CLK_BIT_L;                          /*set SCLK high to toggle clock*/
    count = count^0x800000;                               /* Read the MSB */
    CLK_PIN_L->OUT &= ~(CLK_BIT_L);                       /*set SCLK low to toggle clock*/

    return count;
}

unsigned long readCount2(void)
{
    unsigned long count = 0;
    uint32_t i;

    //if !(DATA_PIN_R->IN & DATA_BIT_R)                   /*stay in this loop if DT high*/
    //    return 0;

    for (i = 0; i < 24; i++) {
        CLK_PIN_R->OUT |= CLK_BIT_R;                      /*set SCLK high to initialize data collection*/
        count <<= 1;
        CLK_PIN_R->OUT &= ~(CLK_BIT_R);                   /*set SCLK low to toggle clock*/
        if (DATA_PIN_R->IN & DATA_BIT_R)
            count++;
    }

    CLK_PIN_R->OUT |= CLK_BIT_R;                          /*set SCLK high to toggle clock*/
    count = count^0x800000;                             /* Read the MSB */
    CLK_PIN_R->OUT &= ~(CLK_BIT_R);                       /*set SCLK low to toggle clock*/
    return count;
}
