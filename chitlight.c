// chitlight.c
// first test of wiringpi for shitlight

#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>
#include <math.h>

// SET PIN CONSTANTS
#define CL_DATA 17
#define CL_RESET 24
#define CL_CLOCK 23

void man_cycle_clock(void) {
    digitalWrite(CL_CLOCK,1);
    digitalWrite(CL_CLOCK,0);
}

void man_flush(void) {
    digitalWrite(CL_RESET,1);
    digitalWrite(CL_RESET,0);
}

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

void shiftOut (uint32_t val1, uint32_t val2) {
    // masks out the current flush and clock pins
    int ports_mask_2 = 0b00000010;
    int ports_mask_1 = 0b00000001;
    // defines the clock and flush pins 
    int clck = 0b00010000;
    int flsh = 0b00100000;

    // we'll flush the last 24 bits of val to the shifts

    int i;
    uint8_t data_out1 = 0;
    uint8_t data_out2 = 0;
    for (i=24; i >= 0; i--) {
        data_out1 = ports_mask_1 * ((val2 & (1 << i))!=0);
        data_out2 = ports_mask_2 * ((val1 & (1 << i))!=0);
        digitalWriteByte(data_out1 | data_out2);
        //digitalWriteByte(data_out1 | data_out2 | clck);
        //digitalWriteByte(data_out1 | data_out2);
	man_cycle_clock();
    }
    man_flush();
}

void benchmark (int loops) {
    int i;
    uint32_t val = 0x7;
    for (i=0; i<=loops; i++) {
        shiftOut(val,0x0);
        shiftOut(0x0,0x0);	
    }    
}

void shiftpwm(uint32_t duty_c[32]) {
   int i,j;
   for (i=0; i<60; i++) {
       for (j=0; j<32; j++) {
           shiftOut(duty_c[j],0x0);
       }}
}

uint32_t int2bin(uint8_t value) {
	return pow(2,value)-1; 
}

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
    wiringPiSetupGpio();
    
    // currentl assume that:
    // CLOCK is at GPIO PIN 23
    // RESET is at GPIO PIN 24
    // DATA  is at GPIO PIN 17
    // DATA2 is at GPIO PIN 18

    // Set these PINs to be output
    // We'll have to check if we can use them
    // in the bulk write as well.
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
    // insert 1 into register and shift through
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
    shiftOut(0x0,0x0);
    
}
