// chitlight.c
// first test of wiringpi for shitlight

#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "minwiringPi.h"

// SET PIN CONSTANTS

// set the value to the GPIO pin you connected the reset latch
// and clock to
#define CL_RESET 24
#define CL_CLOCK 23

// this defines a data pin. It is only used
// for debug purposes and shouldn't affect
// the normal programm!
#define CL_DATA 17


// cycle the clock once to shift the data through the registers
void man_cycle_clock(void) {
    digitalWrite(CL_CLOCK,1);
    digitalWrite(CL_CLOCK,0);
}

// activate the data in the registers by cycling the reset latch
// (it's actually not called reset, but I keep calling it that)
void man_flush(void) {
    digitalWrite(CL_RESET,1);
    digitalWrite(CL_RESET,0);
}

// deprecated test function
void man_fast_write(int state) {
    //
    // Simulates a simultaneous write to all DATA pins
    // for benchmark purposes
    //
    // PORTS 17 is 0b0000 0001
    int on = 0b00000001;
    int off= 0b00000000;
    int clck=0b00010000;
    int flsh=0b00100000;
    int st = on;
    if (st==0) st=off;

    digitalWriteByte(st);
    digitalWriteByte(st | clck);
    digitalWriteByte(st | flsh);
    digitalWriteByte(st);
}

// our current main data function. shifts the 24 least significant
// bits of valN to the shift registers behind dataN.
void shiftOut (uint32_t val1, uint32_t val2) {
    // these mark the position of the GPIO pins we want to use in the memory
    // set to 1 where you want to use the pin as appropiate.
    // the order is
    // 0b [4] [25] [24] [23] [22] [27] [18] [17]
    // e.g. 0b00000001 will use port 17
    // n.b. port 25 seems to not work in this setup. However, one should check
    // again when sober, did not find any reason for this behaviour.
    int ports_mask_2 = 0b00000010;  // GPIO 18
    int ports_mask_1 = 0b00000001;  // GPIO 17
    // defines the clock and flush pins - currently unused
    // but maybe we can get a little bit more speed by using them...
    int clck = 0b00010000;
    int flsh = 0b00100000;

    // we'll flush the last 24 bits of val to the shifts

    int i;
    uint8_t data_out1 = 0; // create empty ints
    uint8_t data_out2 = 0;
    // loop through every (binary) position of the value
    for (i=24; i >= 0; i--) {
        // extract the bit at position i out of valN, check if it's
        // 1 || 0, multiply the result (ie leave the mask and turn on DATA
        // or set mask to 0
        data_out1 = ports_mask_1 * ((val2 & (1 << i))!=0);
        data_out2 = ports_mask_2 * ((val1 & (1 << i))!=0);
        // combine the data matrixes and write to GPIO
        digitalWriteByte(data_out1 | data_out2);
          //digitalWriteByte(data_out1 | data_out2 | clck);
          //digitalWriteByte(data_out1 | data_out2);
        // currently manipulating only clock pin here is faster
        // if we patch digitalWriteByte we might shave a cpu cycle or two here...
	man_cycle_clock();
    }
    // everything is in the shift registers, so we can latch the reset
    // unrolling the loop and patching digitalWriteByte again might perform faster...
    man_flush();
}

// benchmark function
// cycles the first LED on DATA1 white and off, but it is the same
// as changing all LEDs at once...
void benchmark (int loops) {
    int i;
    uint32_t val = 0x7;
    for (i=0; i<=loops; i++) {
        shiftOut(val,0x0);
        shiftOut(0x0,0x0);	
    }    
}

// w.i.p. function, intended to dump
// preformated data of pwm-brightnesses
// to the shift registers.
void shiftpwm(uint32_t duty_c[32]) {
   int i,j;
   for (i=0; i<60; i++) {
       for (j=0; j<32; j++) {
           shiftOut(duty_c[j],0x0);
       }}
}

// generate a bit-representation of a
// duty-cycle of width value.
uint32_t int2bin(uint8_t value) {
	return pow(2,value)-1; 
}

// demo function
// cycle beautifu colors on DATA1 and DATA2
void pwm_benchmark (int loops) {
   //uint32_t init = millis();
   int cycle = 256;
   int bigcycle = 1;
   int i,j;
   int k,l;
   int o,p,q;
   uint32_t value,value2;
   uint32_t r,g,b;
   for (l=0; l<=loops; l++) { 
//   for (k=0; k<=32; k++) {
      value = 0x0;
  //     for (j=0; j<=cycle; j++) {
 // blue channel 
       for (o=0; o<=cycle; o++) { 
 // green channel
   for (i=0; i<=bigcycle; i++) {
//       for (p=0; p<=o; p++) {
 // red channel 
//      for (q=0; q<=p; q++) {
       // pwm loop
       for (j=0; j<=cycle; j++){
		   value = 0x0;
		   value2 = 0x0;
		   if (o<j) value |= (1<<2); //...100
//		   if (p<j) value |= (1<<1); //...010
	           if (o>=j) value |= (1<<0);
		   if (o<j) value2 |= (1<<1);
		   if (o>=j) value2 |= (1<<2);
//		   if (q<j) value |= (1<<0); //...001
		   shiftOut(value,value2);
	   }
       }
   }
       for (o=0; o<=cycle; o++) {
   for (i=0; i<=bigcycle; i++) {
          for (j=0; j<=cycle; j++) {
	      value = 0;
	      value2 =0;
	      if (o>=j) value |= (1<<2);
	      if (o<j) value |= (1<<0);
      	      if (o>=j) value2 |= (1<<1);
	      if (o<j) value2 |= (1<<2);
	      shiftOut(value,value2);
	  }
       }
       }
//   }
//   }  
   }   
} 

void blue() {
    shiftOut(0b00000000100100100100100100100100,0x0);
}

void green() {
    shiftOut(0b00000000010010010010010010010010,0x0);
}

void red() {
    shiftOut(0b00000000001001001001001001001001,0x0);
}

void white() {
    shiftOut(0b00000000111111111111111111111111,0x0);}

int main(void) {
    // Prepare the memory for GPIO access, etc
    wiringPiSetup();
    
    // currentl assume that:
    // CLOCK is at GPIO PIN 23
    // RESET is at GPIO PIN 24
    // DATA  is at GPIO PIN 17
    // DATA2 is at GPIO PIN 18

    // Set these PINs to be output
    // Must be done on all GPIO PINs you want to use
    pinMode(23, OUTPUT);
    pinMode(24, OUTPUT);
    pinMode(25, OUTPUT);
    pinMode(17, OUTPUT);
    pinMode(18, OUTPUT);
    pinMode(27, OUTPUT);
    pinMode(4, OUTPUT);
    

    int i;

    // Reset all LEDs

    // set DATA to low
    digitalWrite(CL_DATA, 0);
    for (i=0; i<24; i++) {
	// cycle clock 24 times
        man_cycle_clock();
    }

    // flush
    man_flush();

    uint32_t loops = 1000;
    
    uint32_t begin = micros();

    pwm_benchmark(loops);
    red();
    delay(10000);
    green();
    delay(10000);
    blue();
//    white();
    delay(30000);
    uint32_t end = micros();
    printf("%d\n", (end-begin));

    // Lights off
    shiftOut(0x0,0x0);
    
}
