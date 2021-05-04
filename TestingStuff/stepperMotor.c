#include "msp.h"
#include <stdint.h>
#include "motorDriver.h"
#include "delay.h"
#include "stepperMotor.h"

void initStepperMotor()
{
    DIR_PIN->SEL1 &= ~(DIR_BIT);                    /* Use GPIO */
    DIR_PIN->SEL0 &= ~(DIR_BIT);
    DIR_PIN->DIR |= DIR_BIT;
    PUL_PIN->SEL1 &= ~(PUL_BIT);
    PUL_PIN->SEL0 &= ~(PUL_BIT);
    PUL_PIN->DIR |= PUL_BIT;
    ENA_PIN->SEL1 &= ~(ENA_BIT);
    ENA_PIN->SEL0 &= ~(ENA_BIT);
    ENA_PIN->DIR |= ENA_BIT;
    ENA_PIN->OUT &= ~(ENA_BIT);
}

//runtime is in milliseconds
void MoveLeft(uint32_t runtime, uint32_t clock_freq)
{
    uint32_t x;

    DIR_PIN->OUT &= ~(DIR_BIT);
    for (x = 0; x<runtime; x++)
    {
          PUL_PIN->OUT |= PUL_BIT; //step pin high
          int doug;
          for (doug=0; doug< 1; doug++)
          {
              delay_us(500, clock_freq);
          }
          PUL_PIN->OUT &= ~(PUL_BIT); //step pin high
          int caut;
          for (caut=0; caut< 1; caut++)
          {
              delay_us(500, clock_freq);
          }
    }
}

void MoveRight(uint32_t runtime, uint32_t clock_freq)
{
    uint32_t x;

    DIR_PIN->OUT |= DIR_BIT;
        for (x = 0; x<runtime; x++)
        {
              PUL_PIN->OUT |= PUL_BIT; //step pin high
              int doug;
              for (doug=0; doug< 1; doug++)
              {
                  delay_us(500, clock_freq);
              }
              PUL_PIN->OUT &= ~(PUL_BIT); //step pin high
              int caut;
              for (caut=0; caut< 1; caut++)
              {
                  delay_us(500, clock_freq);
              }
        }


}
