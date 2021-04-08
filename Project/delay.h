/*
 * delay.h
 *
 *  Created on: Mar 10, 2021
 *      Author: Schuyler
 */

#ifndef DELAY_H_
#define DELAY_H_

#include "msp.h"

#define DELAY_FOR_1_MHZ 1000
#define FREQ_1_5MHZ 1
#define FREQ_3MHZ 3
#define FREQ_6MHZ 6
#define FREQ_12MHZ 12
#define FREQ_24MHZ 24
#define FREQ_48MHZ 48

void delay_ms(uint32_t ms, uint32_t freq);
void delay_us(uint32_t us, uint32_t freq);
void set_DCO(uint32_t freq);


#endif /* DELAY_H_ */
