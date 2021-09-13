/*
 * LoadCellDataCollection.h
 *
 *  Created on: Apr 6, 2021
 *      Author: Schuyler
 */

#ifndef LOADCELLDATACOLLECTION_H_
#define LOADCELLDATACOLLECTION_H_

#define DATA_PIN_L P3
#define DATA_BIT_L BIT2
#define CLK_PIN_L P3
#define CLK_BIT_L BIT3

#define DATA_PIN_R P6
#define DATA_BIT_R BIT5
#define CLK_PIN_R P6
#define CLK_BIT_R BIT4

#define CALFACTOR 29090

void initLoadCells();
void initOffsets();
unsigned long readCount1();
unsigned long readCount2();
float readLoadCells();

#endif /* LOADCELLDATACOLLECTION_H_ */
