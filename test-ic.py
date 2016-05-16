from led import LED
from multiprocessing import Process
import shiftpi
import RPi.GPIO as GPIO
import time
import numpy as np

red_cycle = 0.5
green_cycle = 0.7
yellow_cycle = 0.9

red = LED(0)
yellow = LED(1)
green = LED(2)

led_list = []
for i in range(12):
    led_list.append(LED(i))


shiftpi.shiftRegisters(2)

#led_list = [red, yellow, green]



def switch(leds, on, off):
    sort_indices = np.argsort(on)
    leds = np.array(leds)[sort_indices]
    on = np.array(on)[sort_indices]
    if len(sort_indices) > 1:
        sleep_diffs = np.diff(on)

    for led in leds:
        shiftpi.digitalWrite(led.pin(), shiftpi.HIGH) 
    
    time.sleep(on[0])
    shiftpi.digitalWrite(leds[0].pin(), shiftpi.LOW) 

    for i, led in enumerate(leds[1:]):
        time.sleep(sleep_diffs[i])
        shiftpi.digitalWrite(led.pin(), shiftpi.LOW) 
        
    time.sleep(off.min())
 

def get_actives():
    actives = []
    for led in led_list:
        if led.active():
            actives.append(led)
    return actives

def get_inactives():
    inactives = []
    for led in led_list:
        if not led.active():
            inactives.append(led)
    return inactives
    
try:
    t0 = time.time()
#    wait = np.random.random() + 1
    wait = 1.
    while True:
        if time.time() > t0 + wait and len(get_actives()) < 3: #or len(get_actives()) < 1:
            inactives = get_inactives()
            if len(inactives) > 0:
                activate = np.random.choice(inactives)
#                duration = np.round(np.random.random() / 2, 1) + 0.5
                duration = 1.5
                activate.start_one_cycle(duration)
            t0 = time.time()
#            wait = np.random.random() + 1
            wait = 1.5

        actives = get_actives()
        on_list = np.zeros(len(actives))
        off_list = np.zeros(len(actives))
        for i, led in enumerate(actives):
            on, off = led.one_step()
            on_list[i] = on
            off_list[i] = off

        if len(actives) > 0:
            switch(actives, on_list, off_list)

except KeyboardInterrupt:
    shiftpi.digitalWrite(shiftpi.ALL, shiftpi.LOW)
