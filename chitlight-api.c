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
#include "minwiringPi.h"

// some definitions:
#define PLATINEN 2
#define LEDS_PRO_PLATINE 8
#define FARBEN_PRO_LED 3
#define HELLIGKEITSSTUFEN 256
#define FRAMES_IN_BUFFER 256
//MAX. HELLIGKEITSSTUFEN 256 BY CURRENT DESIGN!


// TODO: unify the hardware commands into one external dependency.
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
    size_t count;
    size_t size_elem;
    int pos_write;
    int pos_read;
} ringbuffer;

ringbuffer* init_buffer() {
    ringbuffer* rbf;
    rbf = (ringbuffer*) malloc(sizeof(ringbuffer));
    if (rbf == NULL) return (NULL);
    rbf->pos_write = 1;
    rbf->pos_read = 0;
    rbf->capacity = FRAMES_IN_BUFFER;
    rbf->size_elem = sizeof(t_bufframe);
    return (rbf);
}

uint8_t is_shutdown;

ringbuffer *writer_rbf;


void write_frame(t_memory_frame* frame) {
    size_t counter_shift;
    size_t counter_duty;
    int mask = 0b00000011; // only activate ports that are safe for you!
    for (counter_duty = 0; counter_duty < HELLIGKEITSSTUFEN; counter_duty++) {
        for (counter_shift = 0; counter_shift < (LEDS_PRO_PLATINE * FARBEN_PRO_LED); counter_shift++) {
            // use digitalwritebyte for now, however we could also use our own function
            // where we set all DATA pins to zero and then those to one which should be one before cycling
            digitalWriteByte(frame->cycle[counter_duty].to_gpio[counter_shift] & mask);
            man_cycle_clock();
        }
        man_flush();
    }
}

void *worker (void* p_rbf) {
    t_memory_frame c_frame;
    uint16_t c_rep;
    uint16_t count;
    int next_read;

    //ringbuffer *rbf;
    //rbf = (ringbuffer *) p_rbf;
    printf("Started Thread\n");
    while(!is_shutdown) {
        printf("Started a loop\n");
        printf("Reading Position is at %i\n", writer_rbf->pos_read);
        // get current element from ringbuffer
        c_frame = writer_rbf->buffer[(writer_rbf->pos_read)].frame;
        c_rep = writer_rbf->buffer[(writer_rbf->pos_read)].rep;
        printf("Copied frame from buffer to temporary buffer\n");
        printf("Draw this frame %i times\n", c_rep);
        for (count = 0; count<c_rep; count++) {
            write_frame(&c_frame);
        }
        printf("Succesfully drawn frame\n");
        // check if there is a new frame in the buffer
        // first, increment our read position
        next_read = (writer_rbf->pos_read + 1);
        // check for overflow
        if (next_read > writer_rbf->capacity) next_read = 0; //could be done faster
        // now compare position to head, if they are the same repeat the last frame
        // maybe in reality we need to obtain a lock to be threadsave.
        // but I think because we only read, we just might repeat a frame unnecessarily
        while (next_read==writer_rbf->pos_write) {
            write_frame(&c_frame);
    //        printf("No more frames\n");
        }
        writer_rbf->pos_read = next_read;
        printf("Done drawing frame\n");
    }
    return NULL;
}




float get_fps(void); // gives some calculation about the actual speed of the program.
                     // Might only be available in ltd mode? 

t_memory_frame zero_frame() {
    int count_b, count_c;
    t_memory_frame frame;
    for (count_c = 0; count_c < HELLIGKEITSSTUFEN; count_c++)
        for (count_b = 0; count_b < LEDS_PRO_PLATINE * FARBEN_PRO_LED; count_b++)
            frame.cycle[count_c].to_gpio[count_b] = 0;
    return frame;
}

int init(void) {
    pthread_t worker_thread;
    // does the initialization: creates ring buffer, initializes the gpio pins
    // and starts the thread
    wiringPiSetup();

    pinMode(23, OUTPUT);
    pinMode(24, OUTPUT);
    pinMode(25, OUTPUT);
    pinMode(17, OUTPUT);
    pinMode(18, OUTPUT);
    pinMode(27, OUTPUT);
    pinMode(4, OUTPUT);

    writer_rbf = (ringbuffer*) init_buffer();
    //TODO first frame element will be random numbers. We should set it to zero.
    int i;
    for (i=0; i<FRAMES_IN_BUFFER; i++) {
    writer_rbf->buffer[i].frame = zero_frame();
    writer_rbf->buffer[i].rep = 1;
    }
    is_shutdown = 0; 
    printf("Next Step: Start Thread\n");
    // start thread
    if (pthread_create(&worker_thread, NULL, worker, writer_rbf)) {
        // some error while creating the thread, TODO
        printf("Error creating thread\n");
        return 0;
    }
    printf("Created Thread");
    // success
    return 1;
}

int init_ltd(void); // basically the same, however we will start the thread which will run
                    // in a time limited fashion so we can expect some near constant frames per second


t_bufframe chit2buf(uint16_t rep, t_chitframe* cframe) {
        t_bufframe bff;
        memset(&bff,0,sizeof(bff));
        // TODO: Optimize this
        int c_col, c_led, c_plat, c_rgb;
        for (c_col = 0; c_col < HELLIGKEITSSTUFEN; c_col++) {
            for (c_plat = 0; c_plat < PLATINEN; c_plat++) {
                for (c_led = 0; c_led < LEDS_PRO_PLATINE; c_led++) {
                    for (c_rgb = 0; c_rgb < 3; c_rgb++) {
                        bff.frame.cycle[c_col].to_gpio[3*c_led+c_rgb] |= (cframe->brightness[c_plat][c_led][c_rgb] < c_col) << c_plat;
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
    if (next > writer_rbf->capacity) next=0;
    while (next == writer_rbf->pos_read) {
        // Wait for reader to advance
        sleep(1);
    }
    printf("Inserting frame into buffer at position %i\n", next);
    writer_rbf->buffer[next] = chit2buf(rep, frame);
    // move write head ahead, free access for reader
    writer_rbf->pos_write = next+1;
}


int try_add_frame(uint8_t rep, t_chitframe* frame); // same as add_frame, but don't block. Returns 1 on success and 0 if buffer is full

int shutdown(void); // end the thread and clear the GPIOs

int reset(void); // try to reset the ring buffer and thread if something failed.

int main(void) {
    // demo time
    init();
    int c_pl, c_led, c_rgb;
    t_chitframe red;
    printf("Prepare test frame\n");
    for (c_pl = 0; c_pl < PLATINEN; c_pl++)
        for (c_led = 0; c_led < LEDS_PRO_PLATINE; c_led++)
           for (c_rgb = 0; c_rgb <3; c_rgb++)
               red.brightness[c_pl][c_led][c_rgb] = 0;
    red.brightness[0][6][0]=255;
    red.brightness[1][6][0]=255;
    red.brightness[2][6][0]=255;

    red.brightness[3][6][0]=255;
    red.brightness[4][6][0]=255;
    red.brightness[5][6][0]=255;

    red.brightness[6][6][0]=255;
    red.brightness[7][6][0]=255;

    printf("Send frame to thread\n");
    add_frame((uint16_t)100, &red);
    printf("Go to sleep\n");
    sleep(60);
    return 0;
}

