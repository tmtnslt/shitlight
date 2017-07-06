/*
*
* C API to use the chitlight functionality in Python
* Written by Lorenz
*
*/

// some definitions:
#define PLATINEN 5
#define LEDS_PRO_PLATINE 8
#define FARBEN_PRO_LED 3

typedef struct {
     uint8_t brightness[PLATINEN][LEDS_PRO_PLATINE][FARBEN_PRO_LED];
} t_chitframe;


float get_fps(void); // gives some calculation about the actual speed of the program.
                     // Might only be available in ltd mode? 

int get_fps_limit(void); // returns the value of the fps limit for calculating appropiate repeats

int init(void); // does the initialization: creates ring buffer, initializes the gpio pins
    // and starts the thread

int init_ltd(void); // basically the same, however we will start the thread which will run
                    // in a time limited fashion so we can expect some near constant frames per second

void add_frame(uint16_t rep, uint8_t on_beat, t_chitframe* frame);    // align a frame to optimal memory layout and add to the ring buffer
    // to be drawn (rep) times. This call will block if the ring buffer is full

int try_add_frame(uint8_t rep, uint8_t on_beat, t_chitframe* frame); // same as add_frame, but don't block. Returns 1 on success and 0 if buffer is full

int shutdown(void); // end the thread and clear the GPIOs

int reset(void); // try to reset the ring buffer and thread if something failed.

int set_bpm(float bpm); // set the internal bpm counter

float get_bpm(void); // return value of internal bpm counter

int get_analysis_state(void); // give info about the state of the beat analysis

int init_analysis(void); // start the beat analysis thread

int stop_analysis(void); // stop the beat analysis thread

int beat_sync(uint8_t enabled); // controlls if the frame loop tries to syncronize to bpm and beat signaling.
