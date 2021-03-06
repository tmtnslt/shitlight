/*
*
* C API to use the chitlight functionality in Python
* Written by Lorenz
*
*/

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>

#include "chitlight-api.h"
#include "chitlight-clut.h"
#include "minwiringPi.h"

// some definitions:
#define EXT_HELLIGKEITSSTUFEN 256
#define HELLIGKEITSSTUFEN 256 
#define FRAMES_IN_BUFFER 1024

int mask = 0b00011111; // only activate ports that are safe for you!

// activate framelimiter
#define NOFRAMELIMIT_ACTIVE
// set framelimit to 100fps
// i.e. time to draw a frame is 10000 mu s
#define LIMIT_MICROS 10000
// TODO: unify the hardware commands into one external dependency.
// SET PIN CONSTANTS

// set the value to the GPIO pin you connected the reset latch
// and clock to
#define CL_RESET 11
#define CL_CLOCK 5

// this defines a data pin. It is only used
// for debug purposes and shouldn't affect
// the normal programm!
#define CL_DATA 17

// cycle the clock once to shift the data through the registers
void man_cycle_clock(void) {
    digitalWrite(CL_CLOCK,1);
    delayMicroseconds(1);
    digitalWrite(CL_CLOCK,0);
}

// activate the data in the registers by cycling the reset latch
// (it's actually not called reset, but I keep calling it that)
void man_flush(void) {
    digitalWrite(CL_RESET,1);
//    delayMicroseconds(2);
    digitalWrite(CL_RESET,0);
}



typedef struct {
    uint8_t to_gpio[LEDS_PRO_PLATINE * FARBEN_PRO_LED];
} t_frame_subcycle;

typedef struct {
    t_frame_subcycle cycle[HELLIGKEITSSTUFEN];
} t_memory_frame;

typedef struct {
    t_memory_frame frame;
    uint16_t rep;
} t_bufframe;

typedef struct {
    t_bufframe buffer[FRAMES_IN_BUFFER];
    void *buffer_end;
    size_t capacity;
    // size_t count; // unneeded, we don't count elements in the ringbuffer
    // size_t size_elem; //unneeded, all structs are well definded in memorysize
    int pos_write;
    int pos_read;
} ringbuffer;

ringbuffer* init_buffer() {
    ringbuffer* rbf;
    rbf = (ringbuffer*) malloc(sizeof(ringbuffer));
    if (rbf == NULL) return (NULL);
    // set all memory to zero
    memset(rbf,0,sizeof(ringbuffer));
    // now initialize our elements
    rbf->pos_write = 1;
    rbf->pos_read = 0;
    rbf->capacity = FRAMES_IN_BUFFER;
    // rbf->size_elem = sizeof(t_bufframe); //unneeded, see struct
    return (rbf);
}

// global bool to stop worker thread
uint8_t is_shutdown;

// global counter for buffer underruns in worker thread
int16_t bufferunderruns;

// global float for fps
float fps;

// global pointer to ringbuffer
ringbuffer *writer_rbf;

// global pointer to LUT
uint8_t *clut;

void write_frame(t_memory_frame* frame) {
    // writes a memory aligned frame to the GPIO
    size_t counter_shift;
    size_t counter_duty;
    // mask in only our active data ports, so we dont accidentaly use a CLCK or FLUSH pin
    for (counter_duty = 0; counter_duty < HELLIGKEITSSTUFEN; counter_duty++) {
        for (counter_shift = 0; counter_shift < (LEDS_PRO_PLATINE * FARBEN_PRO_LED); counter_shift++) {
            // use digitalwritebyte for now, however we could also use our own function
            // where we set all DATA pins to zero and then those to one which should be one before cycling
            blk_write_data(frame->cycle[counter_duty].to_gpio[counter_shift] & mask);
            blk_cycle_clock();
        }
        blk_flush();
    }
}

void *worker (void* p_rbf) {
    t_memory_frame c_frame;
    uint16_t c_rep=1;
    uint16_t count;
    int next_read;
    uint32_t begin = micros();
    uint32_t end = micros()+10;
    int32_t left = 0;
    // we'll use the global rbf for now, so no need to use the pointer from function argument
    //ringbuffer *rbf;
    //rbf = (ringbuffer *) p_rbf;
    #ifdef _DEBUG
      printf("Started Thread\n");
    #endif
    // reset global status variables
    fps = 0.0;
    bufferunderruns = 0;

    while(!is_shutdown) {
        end = micros();
        #ifdef _DEBUG
          printf("TIME: %i\t", end);
        #endif
        fps = (float)c_rep/(float)((end-begin)/1000000.0);

        #ifdef _DEBUG
          printf("FPS: %f\n", fps);
        #endif

	begin=end;

        #ifdef _DEBUG
          printf("Started a loop\n");
          printf("Reading Position is at %i\n", writer_rbf->pos_read);
        #endif

        // get current element from ringbuffer
        c_frame = writer_rbf->buffer[(writer_rbf->pos_read)].frame;
        c_rep = writer_rbf->buffer[(writer_rbf->pos_read)].rep;

        #ifdef _DEBUG
          printf("Copied frame from buffer to temporary buffer\n");
          printf("Draw this frame %i times\n", c_rep);
        #endif

        for (count = 0; count<c_rep; count++) {
          write_frame(&c_frame);
        }
        
        #ifdef _VERBOSE
          printf("Succesfully drawn frame\n");
        #endif
        // check if there is a new frame in the buffer
        // first, increment our read position
        next_read = (writer_rbf->pos_read + 1);
        // check for overflow
        if (next_read >= (writer_rbf->capacity)) next_read = 0; //if we've fixed the buffer length, we can bitfiddle here.
        // now compare position to head, if they are the same repeat the last frame
        // maybe in reality we need to obtain a lock to be threadsave.
        // but I think because we only read, we just might repeat a frame unnecessarily
        while (next_read==writer_rbf->pos_write) {
            write_frame(&c_frame);
            bufferunderruns++; // we don't care for overflow, client will have to handle it.

            #ifdef _DEGBUG
              printf("No more frames, buffer underrun\n");
            #endif
        }
        // check if frame limiter is active
        #ifdef FRAMELIMIT_OPTIONAL
        if (limitframes) {
        #endif
        #ifdef FRAMELIMIT_ACTIVE
        if (1) {
        #endif
        #if defined(FRAMELIMIT_OPTIONAL) || defined(FRAMELIMIT_ACTIVE)
            left = (LIMIT_MICROS*c_rep)-(micros()-begin);
            while (left > 1000) {
                // we were fast than required, enough time to draw more frames
                #ifdef _DEBUG
                    printf("Frame limiter, draw additional frames. Time to fill: %i\n", left);
                #endif
                write_frame(&c_frame);
                left = (LIMIT_MICROS*c_rep)-(micros()-begin);
            }
            // still time left, but too much to for a new frame, just burn some time
            if (left > 0) delayMicroseconds(left);
        }
        #endif
        writer_rbf->pos_read = next_read;
        #ifdef _VERBOSE
          printf("Done drawing frame\n");
        #endif
    }
    return NULL;
}




float get_fps(void) {
        // gives some calculation about the actual speed of the program.
        return fps;
} 


int init(void) {
    pthread_t worker_thread;
    // does the initialization: creates ring buffer, initializes the gpio pins
    // and starts the thread
    wiringPiSetup();
    int i;
    printf("%i",BEGIN_DATA_BLOCK);
    // initialize bulk ports
    for (i=0;i<5;i++) {
        pinMode(BEGIN_DATA_BLOCK+i, OUTPUT);
        pinMode(BEGIN_CLOCK_BLOCK+i, OUTPUT);
        pinMode(BEGIN_RESET_BLOCK+i, OUTPUT);
    }

    writer_rbf = (ringbuffer*) init_buffer(); //init sets all frames to zero, including reps. make sure the worker loop doesn't get stuck on zero rep counter.
    is_shutdown = 0; 
    // point global LUT to cie LUT
    clut = clut_cie;
//    clut = clut_linear;

    #ifdef _DEBUG
      printf("Next Step: Start Thread\n");
    #endif

    // start thread
    if (pthread_create(&worker_thread, NULL, worker, writer_rbf)) {
        // some error while creating the thread, TODO
        #ifdef _DEBUG
          printf("Error creating thread\n");
        #endif
        return 0;
    }
    #ifdef _DEBUG
      printf("Created Thread");
    #endif
    // success
    return 1;
}

int init_ltd(void); // basically the same, however we will start the thread which will run
                    // in a time limited fashion so we can expect some near constant frames per second


t_bufframe chit2buf(uint16_t rep, t_chitframe* cframe) {
        t_bufframe bff;
        memset(&bff,0,sizeof(t_bufframe));
        // TODO: Optimize this
        int c_col, c_led, c_plat, c_rgb;
        for (c_col = 0; c_col < HELLIGKEITSSTUFEN; c_col++) {
            for (c_plat = 0; c_plat < PLATINEN; c_plat++) {
                for (c_led = 0; c_led < LEDS_PRO_PLATINE; c_led++) {
                    for (c_rgb = 0; c_rgb < 3; c_rgb++) {
                        #ifdef _VERBOSE
                        if ((cframe->brightness[c_plat][c_led][c_rgb])!=0) {
                        printf("%i",clut[(cframe->brightness[c_plat][c_led][c_rgb])]); }
                        #endif
                        bff.frame.cycle[c_col].to_gpio[(3*(LEDS_PRO_PLATINE-1-c_led)+(2-c_rgb))] |= (clut[(cframe->brightness[c_plat][c_led][c_rgb])] > c_col) << c_plat;
                    }
                }
             }
        }
        bff.rep = rep;
        return bff;
}


void add_frame(uint16_t rep, t_chitframe* frame) {
    // align a frame to optimal memory layout and add to the ring buffer
    // to be drawn (rep) times. This call will block if the ring buffer is full

    int next;
    next = (writer_rbf->pos_write);
    if (next > (writer_rbf->capacity-1)) next=0;
    while (next == writer_rbf->pos_read) {
        // Wait for reader to advance
        sleep(1);
    }
    #ifdef _DEBUG
    printf("Inserting frame into buffer at position %i\n", next);
    #endif
    writer_rbf->buffer[next] = chit2buf(rep, frame);
    // move write head ahead, free access for reader
    writer_rbf->pos_write = next+1;
}


int try_add_frame(uint8_t rep, t_chitframe* frame); // same as add_frame, but don't block. Returns 1 on success and 0 if buffer is full

int shutdown(void); // end the thread and clear the GPIOs

int reset(void); // try to reset the ring buffer and thread if something failed.

float getnorm( float* normvec, int len )
{
     float norm= 0;
     int i;
     for(i=0;i<len;i++)
     {
	norm+= normvec[i]*normvec[i];
     }
     return sqrtf(norm);
}

void getrandvec( float* randvec, int len )
{
     int i;
     do
     {
	for(i=0;i<len;i++)
	    randvec[i] = (1.0-2.0*(float)rand()/(float)RAND_MAX);
     }
     while(getnorm(randvec, len) > 1.0);
}


void getnormvec( float* normvec, int len )
{
     float norm = getnorm(normvec, len);
     while(len--)
	normvec[len] /= norm;
}


int main(void) {
    init();
    t_chitframe f;
    memset(&f, 0, sizeof(t_chitframe));
    int i,j,k,l,flip;
    k=50;
    flip=1;
    l=0;
    while (1) {
       for (j=(-16);j<16;j++) {
          i = abs(j)-4;
          memset(&f, 0, sizeof(t_chitframe));
          if((i-5)>=0)
              f.brightness[0][i-5][l] = 255/32;
          if((i-4)>=0)
              f.brightness[0][i-4][l] = 255/16;
          if((i-3)>=0)
              f.brightness[0][i-3][l] = 255/8;
          if((i-2)>=0) 
              f.brightness[0][i-2][l] = 255/4;
          if((i-1)>=0) 
              f.brightness[0][i-1][l] = 255/2;
          if((i>=0) && (i<8)) 
              f.brightness[0][i][l] = 255;
          if((i+1)<8) 
              f.brightness[0][i+1][l]= 255/2;
          if((i+2)<8) 
              f.brightness[0][i+2][l] = 255/4;
          if((i+3)<8)
              f.brightness[0][i+3][l] = 255/8; 
          if((i+4)<8)
              f.brightness[0][i+4][l] = 255/16; 
          if((i+5)<8)
              f.brightness[0][i+5][l] = 255/32; 
          add_frame((uint16_t)k/5, &f);
       }
       if((k>25) && (flip==1)) {
         k--;
       } else {
         if ((k<125) && (flip==0)) {
           k++;
         }
         else {      
             flip=!flip;
             l++;
             if(l>=3) l=0;
         }
      }
    }
}
int old_main(void) {
    // demo time
    init();
    t_chitframe red;
    memset(&red,0,sizeof(t_chitframe));
  
    printf("Prepare test frame\n");
    int i,j,k;
    float colorvec_r[8];
    float colorvec_g[8];
    float colorvec_b[8];
    float randvec[8];
    float norm_r, norm_g, norm_b;
    float gamma=0.5;
    float variation= .005000000;
    
    memset(colorvec_r, 0, sizeof(colorvec_r));
    memset(colorvec_g, 0, sizeof(colorvec_g));
    memset(colorvec_b, 0, sizeof(colorvec_b));
    colorvec_r[0] = 1.0;
    colorvec_g[5] = 1.0;
    colorvec_b[7] = 1.0;
	
    while(1) {
    for(i=0;i<8;i++) {
	getrandvec(randvec, 8);
	getnormvec(randvec, 8);
	colorvec_r[i] += variation*randvec[i];
	getrandvec(randvec, 8);
	getnormvec(randvec, 8);
	colorvec_g[i] += variation*randvec[i];
	getrandvec(randvec, 8);
	getnormvec(randvec, 8);
	colorvec_b[i] += variation*randvec[i];
    }
    norm_r=0; norm_g=0; norm_b=0;
    for(i=0;i<8;i++) {
	norm_r+=colorvec_r[i]*colorvec_r[i];
	norm_g+=colorvec_g[i]*colorvec_g[i];
	norm_b+=colorvec_b[i]*colorvec_b[i];
    }
    for(i=0;i<8;i++) {
	colorvec_r[i] /= sqrt(norm_r);
	colorvec_g[i] /= sqrt(norm_g);
	colorvec_b[i] /= sqrt(norm_b);
    }
    for(i=0;i<8;i++) {
         red.brightness[0][i][0]= (uint16_t)(powf(fabs(colorvec_r[i]), gamma)*255.0)&0xFF;
         red.brightness[0][i][1]= (uint16_t)(powf(fabs(colorvec_g[i]), gamma)*255.0)&0xFF;
         red.brightness[0][i][2]= (uint16_t)(powf(fabs(colorvec_b[i]), gamma)*255.0)&0xFF;
    }
//    red.brightness[1][6][1]=255;
    //printf("Send frame to thread\n");
    add_frame((uint16_t)1, &red);
//    red.brightness[0][i][0]=0;
    }
    printf("Go to sleep\n");
    return 0;
}






