/*
 * LoadCellDataCollection.h
 *
 *  Created on: Apr 6, 2021
 *      Author: Schuyler
 */

#ifndef LOADCELLDATACOLLECTION_H_
#define LOADCELLDATACOLLECTION_H_

#define DATA_PIN_L P1
#define DATA_BIT_L BIT5
#define CLK_PIN_L P6
#define CLK_BIT_L BIT1
#define OFFSET_L 8388007.0
#define CAL_COUNT_L 706550.0

#define DATA_PIN_R P4
#define DATA_BIT_R BIT3
#define CLK_PIN_R P6
#define CLK_BIT_R BIT4
#define OFFSET_R 8382000.0
#define CAL_COUNT_R 554331.0

#define DOWN 1
#define UP 2
#define CAL_WEIGHT 8
#define BIAS 25.4 //lbs
#define GAIN 146.2

void initLoadCells();
unsigned long readCount1();
unsigned long readCount2();
void readLoadCells(uint32_t threshhold, uint8_t run);

#endif /* LOADCELLDATACOLLECTION_H_ */
