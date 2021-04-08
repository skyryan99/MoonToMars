/*
 * XAxisMotor.h
 *
 *  Created on: April 4, 2021
 *      Author: Jacob Everest
 */

#ifndef XAXISMOTOR_H_
#define XAXISMOTOR_H_

#define pin2out (0x40004c03)
#define pin2dir (0x40004c05)
#define pin1in (0x40004c00)
#define pin1out (0x40004c02)
#define pin1dir (0x40004c04)
#define pin5dir (0x40004c32)
#define pin5out (0x40004c34)

void MoveLeft(uint32_t runtime)
void MoveRight(uint32_t runtime)

#endif
