#include "msp.h"
#include <stdint.h>
#define pin2out (0x40004c03)
#define pin2dir (0x40004c05)
#define pin3out (0x40004c22)
#define pin3dir (0x40004c24)
#define pin5dir (0x40004c32)
#define pin5out (0x40004c34)

/**
 * main.c
 */
int x = 0;
void setup()
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    uint8_t * p2dir = (uint8_t*) pin2dir;

    uint8_t * p3dir = (uint8_t*) pin3dir;
    uint8_t * p5out = (uint8_t*) pin5out;
    uint8_t * p5dir = (uint8_t*) pin5dir;
    *p2dir |= 0b00001100; //set direction of the pin
    *p3dir |= 0b00000001;
    *p5dir |= 0b00000001;
    *p5out |= 0b00000000; //set pin output to low

//    P2DIR = 0b00001000; //Set step pin (pulse pin) as output
//    P3DIR = 0b00000001; //Set Direction pin as output
//    P5DIR = 0b00000001; //Set enable pin as output
//    P5OUT = 0b00000000; //Set enable output to low for now

}
void main(void)
{
    uint8_t * p2dir = (uint8_t*) pin2dir;
    uint8_t * p3dir = (uint8_t*) pin3dir;
    *p2dir |= 0b00001100; //set direction of the pin
    *p3dir |= 0b00000001;
    uint8_t * p2out = (uint8_t*) pin2out;
    uint8_t * p3out = (uint8_t*) pin3out;
    while(1)
    {
        *p3out |= 0b00000001; //Set direction of 3.0 to high

            for (x = 0; x < 20000; x++)
            {
                *p2out |= 0b00001100; //step pin high
                int dog;
                for (dog=0; dog<1; dog++)
                {
                    __delay_cycles(500);
                }
                P2OUT ^= 0b00001100; //step pin high
                int cat;
                for (cat=0; cat<1; cat++)
                {
                    __delay_cycles(500);
                }

            }

            int d;
            for (d=0; d<10; d++)
            {
                __delay_cycles(1000000);
            }
            *p3out ^= 0b00000001; //Set direction of 3.0 to low

            for (x = 0; x < 20000; x++)
            {
                *p2out |= 0b00001100; //step pin high
                int e;
                for (e=0; e<1; e++)
                {
                    __delay_cycles(500);
                }
                *p2out ^= 0b00001100; //step pin high
                int c;
                for (c=0; c<1; c++)
                {
                    __delay_cycles(500);
                }
            }

    }
}
//void loop()
//{
//
//}

