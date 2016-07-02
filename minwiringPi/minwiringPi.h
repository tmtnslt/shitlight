/*
 * minwiringPi.h:
 * based on wiringPi.h, stripped for chitlight by Lorenz. Original license:
 *	Arduino like Wiring library for the Raspberry Pi.
 *	Copyright (c) 2012-2016 Gordon Henderson
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://projects.drogon.net/raspberry-pi/wiringpi/
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

#ifndef	__WIRING_PI_H__
#define	__WIRING_PI_H__

// C doesn't have true/false by default and I can never remember which
//	way round they are, so ...

#ifndef	TRUE
#  define	TRUE	(1==1)
#  define	FALSE	(!TRUE)
#endif


// Settings for use with chitlight

#define BEGIN_DATA_BLOCK 0
#define BEGIN_CLOCK_BLOCK 10
#define BEGIN_RESET_BLOCK 5

#define CLOCK_DELAY 1
// reset delay is currently commented out
#define RESET_DELAY 1




// Handy defines

// wiringPi modes

#define	WPI_MODE_PINS		 0
#define	WPI_MODE_GPIO		 1
#define	WPI_MODE_GPIO_SYS	 2
#define	WPI_MODE_PHYS		 3
#define	WPI_MODE_PIFACE		 4
#define	WPI_MODE_UNINITIALISED	-1

// Pin modes

#define	INPUT			 0
#define	OUTPUT			 1
#define	PWM_OUTPUT		 2
#define	GPIO_CLOCK		 3
#define	SOFT_PWM_OUTPUT		 4
#define	SOFT_TONE_OUTPUT	 5
#define	PWM_TONE_OUTPUT		 6

#define	LOW			 0
#define	HIGH			 1

// Pull up/down/none

#define	PUD_OFF			 0
#define	PUD_DOWN		 1
#define	PUD_UP			 2

// PWM

#define	PWM_MODE_MS		0
#define	PWM_MODE_BAL		1

// Interrupt levels

#define	INT_EDGE_SETUP		0
#define	INT_EDGE_FALLING	1
#define	INT_EDGE_RISING		2
#define	INT_EDGE_BOTH		3

// Pi model types and version numbers
//	Intended for the GPIO program Use at your own risk.

#define	PI_MODEL_A		0
#define	PI_MODEL_B		1
#define	PI_MODEL_AP		2
#define	PI_MODEL_BP		3
#define	PI_MODEL_2		4
#define	PI_ALPHA		5
#define	PI_MODEL_CM		6
#define	PI_MODEL_07		7
#define	PI_MODEL_3		8
#define	PI_MODEL_ZERO		9

#define	PI_VERSION_1		0
#define	PI_VERSION_1_1		1
#define	PI_VERSION_1_2		2
#define	PI_VERSION_2		3

#define	PI_MAKER_SONY		0
#define	PI_MAKER_EGOMAN		1
#define	PI_MAKER_MBEST		2
#define	PI_MAKER_UNKNOWN	3

//	Intended for the GPIO program Use at your own risk.

// Threads

#define	PI_THREAD(X)	void *X (void *dummy)

// Failure modes

#define	WPI_FATAL	(1==1)
#define	WPI_ALMOST	(1==2)

// Function prototypes
//	c++ wrappers thanks to a comment by Nick Lott
//	(and others on the Raspberry Pi forums)

#ifdef __cplusplus
extern "C" {
#endif

// Core wiringPi functions

int  wiringPiSetup       (void) ;

void pinMode             (int pin, int mode) ;
void digitalWrite        (int pin, int value) ;

// On-Board Raspberry Pi hardware specific stuff

void digitalWriteByte    (int value) ;

// Extras from arduino land

void         delay             (unsigned int howLong) ;
void         delayMicroseconds (unsigned int howLong) ;
unsigned int millis            (void) ;
unsigned int micros            (void) ;

void delayMicrosecondsHard (unsigned int howLong);


// New Functions for chitlight

void blk_cycle_clock(void);
void blk_flush(void);
void blk_write_data(int value);

#ifdef __cplusplus
}
#endif

#endif
