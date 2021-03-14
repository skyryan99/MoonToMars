#include "msp.h"
#include "motorDrive.h"
#include "delay.h"

/**
 * main.c
 */
#define pin2out (0x40004c03)
#define pin2dir (0x40004c05)
#define pin1in (0x40004c00)
#define pin1out (0x40004c02)
#define pin1dir (0x40004c04)
#define pin5dir (0x40004c32)
#define pin5out (0x40004c34)

void main(void)
{

    int x;

    uint8_t * p2dir = (uint8_t*) pin2dir;
    uint8_t * p1dir = (uint8_t*) pin1dir;
    uint8_t * p5out = (uint8_t*) pin5out; //connect enable+ to 5.0 pin
    uint8_t * p5dir = (uint8_t*) pin5dir;
    *p2dir |= 0b00001100; //set direction of the pin
    *p1dir |= 0b01000000;
    *p5dir |= 0b00000001;
    *p5out &= 0b00000000; //set pin output to low
    uint8_t * p2out = (uint8_t*) pin2out; //connect pul+ to 2.3 pin
    uint8_t * p1out = (uint8_t*) pin1out; //connect dir+ to 1.6 pin
    uint8_t * p1in = (uint8_t*) pin1in; //connect pul-, dir-, and enable- to the same common ground pin (board ground)

    /*
     * Moves 0.20 inches/sec down
     * Moves 0.14 inches/sec up
     *
     * CW is UP
     * CCW is DOWN
     *
     * Move down 21 in.
     * Stop,
     * reverse direction,
     * move up 21 in.
     * Stop permanently
     *
     * NOTES:
     *  -Going up went up 6 inches less than going down
     *
     */

    uint32_t clock_freq = FREQ_24MHZ;
    set_DCO(clock_freq);
    initMotorDrive();

    //setSpeed(-1000);        //100% down
    //delay_ms(150000, clock_freq);    //140s

    //setSpeed(0);        //off
    //delay_ms(3000, clock_freq);         //5s

    //setSpeed(1000);         //100% up
    //delay_ms(150000, clock_freq);       //140s

    //setSpeed(0);        //off

    *p1out &= ~0b01000000; //when this is set to zero, system will move to the right when turned on
    for (x = 0; x<10000; x++)
    {
          *p2out |= 0b00001100; //step pin high
          int doug;
          for (doug=0; doug< 1; doug++)
          {
              delay_us(500, clock_freq);
          }
          *p2out ^= 0b00001100; //step pin high
          int caut;
          for (caut=0; caut< 1; caut++)
          {
              delay_us(500, clock_freq);
          }
    }
    *p1out ^= 0b01000000; //when this is set to zero, system will move to the right when turned on
        for (x = 0; x<10000; x++)
        {
              *p2out |= 0b00001100; //step pin high
              int doug;
              for (doug=0; doug< 1; doug++)
              {
                  delay_us(500, clock_freq);
              }
              *p2out ^= 0b00001100; //step pin high
              int caut;
              for (caut=0; caut< 1; caut++)
              {
                  delay_us(500, clock_freq);
              }
        }

    *p2out &= ~0b00001100; //step pin low


}
