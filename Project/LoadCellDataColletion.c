#include "msp.h"
#include "LoadCellDataCollection.h"
#include <stdio.h>

/**
 * 1. Provide 25 clock input signals to set input channel A to a gain of 128
 * 2. Initiate data collection by sending SDA low
 * 3. Transfer bits
 */

unsigned long readCount1(void);
unsigned long readCount2(void);

void readLoadCells(float threshold, uint32_t run)
{
    unsigned char test = 1;
    uint32_t val1;
    uint32_t val2;
    //uint32_t left_val;
    //uint32_t right_val;
    //uint32_t BUFFER[30];
    uint32_t mied=0;
    uint32_t offset=0;
    uint8_t j = 0;
    float WOB=0;

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    if (run == 1) {
        while(1){
            while(j<20){
                val1 = readCount1();
                //left_val = (val1 - 8388007.0)/(706550.0*8);
                val2 = readCount2();
                //right_val = (val2 - 8382000.0)/(554331.0*8);
                //BUFFER[j]=val2;
                WOB = ((((val1 - OFFSET_L)/(CAL_COUNT_L*CAL_WEIGHT))+((val2 - OFFSET_R)/(CAL_COUNT_R*CAL_WEIGHT))) + 0.16) * 168.5; // Add both load cell values
                //printf("WOB [N]: %.2f \n\r",(((val1 - OFFSET_L)/(CAL_COUNT_L*CAL_WEIGHT))+((val2 - OFFSET_R)/(CAL_COUNT_R*CAL_WEIGHT)))*4.45*9.81);// Print both values from LOAD CELL PLATFORM
                printf("WOB [lbs]: %.2f \n\r",WOB);
                if (WOB > threshold)
                    return;
                //mied+=BUFFER[j];
                j++;
            }
            j=0;
            mied/=20;

            if (test==0){
                offset=mied;
                test=1;
            }
            //mied-=offset;

            //printf("mied=%lu \r\n",mied);
            //zero 8369500
        }
    }
    else if (run == 2) {
        while(1){
            while(j<20){
                val1 = readCount1();
                //left_val = (val1 - 8388007.0)/(706550.0*8);
                val2 = readCount2();
                //right_val = (val2 - 8382000.0)/(554331.0*8);
                //BUFFER[j]=val2;
                WOB = ((((val1 - OFFSET_L)/(CAL_COUNT_L*CAL_WEIGHT))+((val2 - OFFSET_R)/(CAL_COUNT_R*CAL_WEIGHT))) + 0.16) * 168.5; // Add both load cell values
                //printf("WOB [N]: %.2f \n\r",(((val1 - OFFSET_L)/(CAL_COUNT_L*CAL_WEIGHT))+((val2 - OFFSET_R)/(CAL_COUNT_R*CAL_WEIGHT)))*4.45*9.81);// Print both values from LOAD CELL PLATFORM
                printf("WOB [lbs]: %.2f \n\r",WOB);
                if (WOB < threshold)
                    return;
                //mied+=BUFFER[j];
                j++;
            }
            j=0;
            mied/=20;

            if (test==0){
                offset=mied;
                test=1;
            }
            //mied-=offset;

            //printf("mied=%lu \r\n",mied);
            //zero 8369500
        }
    }
}

unsigned long readCount1(void)
{
    unsigned long count;
    unsigned char i;
    /* Input - DT - on P1.5*/
    DATA_PIN_L->SEL1 &= ~(DATA_BIT_L);                   /* Use GPIO */
    DATA_PIN_L->SEL0 &= ~(DATA_BIT_L);
    DATA_PIN_L->DIR &= ~(DATA_BIT_L);                    /*0 - input*/
    DATA_PIN_L->REN |= DATA_BIT_L;                       /*enable pull resistor*/
    DATA_PIN_L->OUT |= DATA_BIT_L;                       /*select pull UP resistor*/

    /* Output - SCK - on P2.6*/
    CLK_PIN_L->SEL1 &= ~(CLK_BIT_L);                     /* Use GPIO */
    CLK_PIN_L->SEL0 &= ~(CLK_BIT_L);
    CLK_PIN_L->DIR |= CLK_BIT_L;                         /*1 - output*/

    DATA_PIN_L->IN | DATA_BIT_L;                         /*set DT high*/
    CLK_PIN_L->OUT &= ~(CLK_BIT_L);                      /*set SCK low */
    count = 0;

    while(DATA_PIN_L->IN & DATA_BIT_L);                  /*stay in this loop if DT high*/

    for(i=0;i<24;i++){
        CLK_PIN_L->OUT |= CLK_BIT_L;                      /*set SCLK high to initialize data collection*/
        count<<=1;
        CLK_PIN_L->OUT &= ~(CLK_BIT_L);                   /*set SCLK low to toggle clock*/
        if(DATA_PIN_L->IN & DATA_BIT_L) count++;
        }
    CLK_PIN_L->OUT |= CLK_BIT_L;                          /*set SCLK high to toggle clock*/
    count = count^0x800000;                               /* Read the MSB */
    CLK_PIN_L->OUT &= ~(CLK_BIT_L);                       /*set SCLK low to toggle clock*/
    return(count);
}

unsigned long readCount2(void)
{
    unsigned long count2;
    unsigned char z;
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
    count2 = 0;

    while(DATA_PIN_R->IN & DATA_BIT_R);                   /*stay in this loop if DT high*/

    for(z=0;z<24;z++){
        CLK_PIN_R->OUT |= CLK_BIT_R;                      /*set SCLK high to initialize data collection*/
        count2<<=1;
        CLK_PIN_R->OUT &= ~(CLK_BIT_R);                   /*set SCLK low to toggle clock*/
        if(DATA_PIN_R->IN & DATA_BIT_R) count2++;
        }
    CLK_PIN_R->OUT |= CLK_BIT_R;                          /*set SCLK high to toggle clock*/
    count2 = count2^0x800000;                             /* Read the MSB */
    CLK_PIN_R->OUT &= ~(CLK_BIT_R);                       /*set SCLK low to toggle clock*/
    return(count2);
}
