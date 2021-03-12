#include "msp.h"
#include <stdint.h>
#define pin2out (0x40004c03)
#define pin2dir (0x40004c05)
#define pin3in (0x40004c20)
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
    *p5out &= 0b00000000; //set pin output to low
}
void main(void)
{
    uint8_t * p2dir = (uint8_t*) pin2dir;
    uint8_t * p3dir = (uint8_t*) pin3dir;
    uint8_t * p5out = (uint8_t*) pin5out; //connect enable+ to 5.0 pin
    uint8_t * p5dir = (uint8_t*) pin5dir;
    *p2dir |= 0b00001100; //set direction of the pin
    *p3dir |= 0b00000001;
    *p5dir |= 0b00000001;
    *p5out &= 0b00000000; //set pin output to low
    uint8_t * p2out = (uint8_t*) pin2out; //connect pul+ to 2.3 pin
    uint8_t * p3out = (uint8_t*) pin3out; //connect dir+ to 3.0 pin
    uint8_t * p3in = (uint8_t*) pin3in; //connect pul-, dir-, and enable- to the same common ground pin (board ground)
    while(1)
    {
//        if (*p3in & 0b01000000) //Pin 3.6 when uncommented can be connected to ground to turn the system off and pwr 3.3V to turn the system on (uncomment lines 80 - 84 to make this work)
//        {
//            *p3out |= 0b00000001; //Set direction of 3.0 to high <--Output Pin 3.0 is set high to move system left when looking from the front
//
//                for (x = 0; x < 3200; x++) // this loop runs it left (when the center is x<1000 it runs for 1 second so you do the math)
//                {
//                    *p2out |= 0b00001100; //step pin high (turn it off
//                    int dog;
//                    for (dog=0; dog< 1; dog++)
//                    {
//                        __delay_cycles(1500);
//                    }
//                    P2OUT ^= 0b00001100; //step pin high
//                    int cat;
//                    for (cat=0; cat< 1; cat++)
//                    {
//                        __delay_cycles(1500);
//                    }
//                }
             *p3out &= 0x00; //when this is set to zero, system will move to the right when turned on
                 for (x = 0; x<3200; x++)
                 {
                       *p2out |= 0b00001100; //step pin high
                       int doug;
                       for (doug=0; doug< 1; doug++)
                       {
                           __delay_cycles(1500);
                       }
                       *p2out ^= 0b00001100; //step pin high
                       int caut;
                       for (caut=0; caut< 1; caut++)
                       {
                           __delay_cycles(1500);
                       }
                 }

//        }
//        else //this else statement needs to be ucommented to have pin 3.6 turn on and off the system
//        {
//            *p2out &= 0x00;
//        }
            }


}
//void loop()
//{
//
//}

