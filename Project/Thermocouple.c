#include "msp.h"
#include "Thermocouple.h"
#include <stdio.h>

/*
 * main.c
 * datasheet: https://cdn-shop.adafruit.com/datasheets/MAX31855.pdf
 * resources: https://users.wpi.edu/~sullivan/ME3901/Laboratories/03-Temperature_Labs/Temperature_an043.pdf
 *
 *
 * SETUP:
 * Thermocouple requires MAX31855 digital converter:
 *   - uses 3.3 VDC
 *   - CS - chip-select                               [any GPIO pin]
 *   - DO - MISO "master in, slave out' - data signal [refer to user manual to determine which pin to use]
 *   - CLK - clock                                    [refer to user manual to determine which pin to use]
 * Output is in degree C
 *
 * Created on: May 21, 2021
 *      Author: Rebecca Rodriguez
 */


int16_t MAX31855ReadTemperature(void);
void delayMs(int n);

void initThermocouple()
{
    EUSCI_B0->CTLW0 = 0x0001;   /*disable UCB0 during config */

    EUSCI_B0->CTLW0 = 0xA9C1;   /*clock phase/polarity:01, MSB first, 8-bit, master, 3-pin SPI, synchronous mode, use SMCLK as clock source */
    EUSCI_B0->BRW = 30;         /*set clock speed*/
    EUSCI_B0->CTLW0 &= ~0x0001; /*Enable UCB0 after config */

    P1->SEL0 |= 0xA0;           /* P1.5, P1.7 for UCB0CLK, UCB0MISO */
    P1->SEL1 &= ~0xA0;          /* Only certain pins may be used for UCB0CLK, UCB0MISO */

    CS_PIN->DIR |= CS_BIT;      /* set CS as output for slave select */
    CS_PIN->OUT |= CS_BIT;      /* slave select (aka "chip select") idle high*/
}

//int main(void){
void print_temp_data(void){
    int32_t temp;

    while(1){
        temp = MAX31855ReadTemperature();
        printf("Temp [°C]: %d\n\r",temp);
        //delayMs(237);
    }
}

int16_t MAX31855ReadTemperature(void)
{
    uint32_t readresults[4];
    uint8_t count;
    volatile int16_t thermocouple_temp;

    count = 4;
    while(count != 0)
    {
        count--;
        P2->OUT &= ~8;                                /*Enable CS*/
        while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));  /*wait for transmit buffer empty */
        EUSCI_B0->TXBUF = 0x00;                       /*Transmitting dummy byte to MAX31855*/
        while(!(EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG));  /*wait for transmit done */
        readresults[count] = EUSCI_B0->RXBUF;         /*Receive data*/
    }

    P2->OUT |= 8;                                     /* deassert slave select aka "chip select"*/
    thermocouple_temp = (int)((((readresults[3]) << 8) | ((readresults[2]) & 0xFC))>> 2)* CAL_CONST; // Retrieve 2's complement

    return thermocouple_temp;
}

void delayMs(int n)
{
    int i,j;
    for(j = 0; j < n; j++)
    {
        for(i = 250; i > 0; i--);       /* Delay 1ms */
    }
}
