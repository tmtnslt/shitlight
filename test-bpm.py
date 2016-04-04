import RPi.GPIO as GPIO
import time
import numpy as np

red = 18
yellow = 23
green = 24

max_bpm = 144
min_bpm = 80 

colors = np.array([red, yellow, green])
button = 25
current_state = False

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

GPIO.setup(button, GPIO.IN)
for color in colors:
    GPIO.setup(color, GPIO.OUT)
    GPIO.output(color, GPIO.LOW)

def switch(state, mask=np.array([True, True, True])):
    if state:
        for color in colors[mask]:
            GPIO.output(color, GPIO.HIGH)
    else:
        for color in colors[mask]:
            GPIO.output(color, GPIO.LOW)


def get_bpm():
    print "Tap to get bpm"
    counts = 0
    time_seq = []
    while counts < 64:
        if GPIO.input(button) == False:
            counts += 1
            time_seq.append(time.time())
            if counts > 1:
                print "BPM:", 60. / np.diff(time_seq).mean()

            # Flash quickly according to pressing of button
            switch(True)
            time.sleep(60. / max_bpm / 4.)
            switch(False)
            time.sleep(60. / max_bpm / 4. * 3)

    delta = np.diff(time_seq).mean()
    t0 = time_seq[0] + counts * delta

    print "BPM:", 60. / delta
    return t0, delta


def blink(t0, delta, min_count=4):
    count = 0
    while True:
        if t0 + delta < time.time():
            t0 = time.time()

            # Create a mask for randomly selectin LED to switch on and off
            mask = np.random.random(3) > 0.5
            while mask.sum() < 1:
                mask = np.random.random(3) > 0.5

            switch(True, mask)

            # Corresponds to time.sleep() but waits for pressing of button
            while (t0 + delta / 2) > time.time():
                if GPIO.input(button) == False and count > min_count:
                    switch(False)
                    time.sleep(60. / max_bpm)
                    return 

            switch(False, mask)
            count += 1

        if GPIO.input(button) == False and count > min_count:
            switch(False)
            time.sleep(60. / max_bpm)
            return 

while True:
    try:
        t0, delta = get_bpm()
        t0 = blink(t0, delta)

    except KeyboardInterrupt:
        GPIO.cleanup()

   

