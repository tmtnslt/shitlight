/*
*
* C API to use the chitlight functionality in Python
* Written by Lorenz
*
*/

// some definitions:
#define PLATINEN 2
#define LEDS_PRO_PLATINE 8
#define FARBEN_PRO_LED 3
#define HELLIGKEITSSTUFEN 256
//MAX. HELLIGKEITSSTUFEN 256 BY CURRENT DESIGN!

typedef struct t_chitframe {
     uint8_t brightness[PLATINEN][LEDS_PRO_PLATINE][FARBEN_PRO_LED];
}

extern float get_fps(void); 

extern int init(void);
extern int init_ltd(void);

extern void add_frame(rep, t_chitframe* frame);
extern int try_add_frame(rep, t_chitframe* frame);
extern int shutdown(void);
extern void reset(void);
