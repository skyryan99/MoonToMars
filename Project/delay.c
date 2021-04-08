/*
 * delay.c
 *
 *  Created on: Mar 10, 2021
 *      Author: Schuyler
 */

#include "delay.h"
#include "msp.h"

void delay_ms(uint32_t ms, uint32_t freq) {
    int i, j;
    if (freq == 1) {
        for (i = 0; i < ms; i++)
            __delay_cycles(1500);           //1.5 freq
        return;
    }

    if (freq == 48)
        freq = 24;

    for (i = 0; i < ms; i++)
        for (j = 0; j < freq; j++)
            __delay_cycles(DELAY_FOR_1_MHZ);   // delay 1 ms
}

void delay_us(uint32_t us, uint32_t freq) {
    int i, j;
    if (freq == 1) {
        us /= 10;
        for (i = 0; i < us; i++)
            __delay_cycles(15);           //1.5 freq
        return;
    }

    if (freq == 48)
        freq = 24;

    for (i = 0; i < us; i++)
        for (j = 0; j < freq; j++)
            __delay_cycles(1);   // delay 1 us
}

void set_DCO(uint32_t freq) {
    uint32_t DCORSEL_val;
    uint32_t CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
    if(freq == 48) {
        /* Transition to VCORE Level 1: AM0_LDO --> AM1_LDO */
        while ((PCM->CTL1 & PCM_CTL1_PMR_BUSY));
        PCM->CTL0 = PCM_CTL0_KEY_VAL | PCM_CTL0_AMR_1;
        while ((PCM->CTL1 & PCM_CTL1_PMR_BUSY));
        /* Configure Flash wait-state to 1 for both banks 0 & 1 */
        FLCTL->BANK0_RDCTL = (FLCTL->BANK0_RDCTL &
                ~(FLCTL_BANK0_RDCTL_WAIT_MASK)) | FLCTL_BANK0_RDCTL_WAIT_1;
        FLCTL->BANK1_RDCTL = (FLCTL->BANK0_RDCTL &
                ~(FLCTL_BANK1_RDCTL_WAIT_MASK)) | FLCTL_BANK1_RDCTL_WAIT_1;

        /* Configure DCO to 48MHz, ensure MCLK uses DCO as source*/
        CS->KEY = CS_KEY_VAL ; // Unlock CS module for register access
        CS->CTL0 = 0; // Reset tuning parameters
        CS->CTL0 = CS_CTL0_DCORSEL_5; // Set DCO to 48MHz
        /* Select MCLK = DCO, no divider */
        CS->CTL1 = CS->CTL1 & ~(CS_CTL1_SELM_MASK | CS_CTL1_DIVM_MASK) |
         CS_CTL1_SELM_3;
        CS->KEY = 0; // Lock CS module from unintended accesses
        return;
    }
    switch(freq){
        case 1:
            DCORSEL_val = CS_CTL0_DCORSEL_0;
        break;
        case 3:
            DCORSEL_val = CS_CTL0_DCORSEL_1;
        break;
        case 6:
            DCORSEL_val = CS_CTL0_DCORSEL_2;
        break;
        case 12:
            DCORSEL_val = CS_CTL0_DCORSEL_3;
        break;
        case 24:
            DCORSEL_val = CS_CTL0_DCORSEL_4;
        break;
        default://48MHz

        break;
    }
    // change DC0 from default of 3MHz
    CS->KEY = CS_KEY_VAL; // unlock CS registers
    CS->CTL0 = 0; // clear register CTL0
    CS->CTL0 = DCORSEL_val; // set DCO = x MHz

    // select clock sources
    CS->CTL1 = CTL1;
    CS->KEY = 0; // lock the CS registers
}
