#include "msp.h"
#include <stdint.h>
#include "motorDrive.h"
#include "delay.h"

//runtime is in milliseconds
void MoveLeft(uint32_t runtime)
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

    *p1out &= ~0b01000000; //when this is set to zero, system will move to the right when turned on
    for (x = 0; x<runtime; x++)
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
}

void MoveRight(uint32_t runtime)
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

    *p1out ^= 0b01000000; //when this is set to zero, system will move to the right when turned on
        for (x = 0; x<runtime; x++)
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


}


