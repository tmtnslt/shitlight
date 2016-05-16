import RPi.GPIO as GPIO
import time
import numpy as np
import sys

# Define pins
#_SER_pin   = 25   #pin 14 on the 75HC595
#_RCLK_pin  = 24   #pin 12 on the 75HC595
#_SRCLK_pin = 23   #pin 11 on the 75HC595

# Define pins
_SER_pin   = 17   #pin 14 on the 75HC595 
_RCLK_pin  = 24   #pin 12 on the 75HC595
_SRCLK_pin = 23   #pin 11 on the 75HC595


GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False) 

GPIO.setup(_SER_pin, GPIO.OUT)
GPIO.setup(_RCLK_pin, GPIO.OUT)
GPIO.setup(_SRCLK_pin, GPIO.OUT)

n_channel = 24

def reset():
    for i in range(n_channel-1, -1, -1):
        GPIO.output(_SER_pin, GPIO.LOW)
        GPIO.output(_SRCLK_pin, GPIO.HIGH) 
        GPIO.output(_SRCLK_pin, GPIO.LOW) 
    GPIO.output(_RCLK_pin, GPIO.HIGH) 
    GPIO.output(_RCLK_pin, GPIO.LOW) 

reset()

def one_cycle(settings):
    for bit in range(0, 256, 8):
        mask = settings <= bit
        for i in range(n_channel-1, -1, -1):
            if mask[i] == True:
                GPIO.output(_SER_pin, GPIO.LOW)
            else:
                GPIO.output(_SER_pin, GPIO.HIGH)

            GPIO.output(_SRCLK_pin, GPIO.HIGH) 
            GPIO.output(_SRCLK_pin, GPIO.LOW) 

        GPIO.output(_RCLK_pin, GPIO.HIGH) 
        GPIO.output(_RCLK_pin, GPIO.LOW) 


while True:
    try:
#        for l in range(3):
#            for j in range(8):
#                brights = np.zeros(n_channel)
#                brights[l+j*3] = 255 #32 * (j + 1) - 1
#                brights[n_channel-(3-l+j*3)] = 255#32 * (j + 1) - 1
#                for _ in range(4):
#                    one_cycle(brights)
#
#            for j in range(8):
#                brights = np.zeros(n_channel)
#                brights[l+j*3] = 255#32 * (j + 1) - 1
#                brights[((l+1)%3)+j*3] = 255#32 * (j+1) - 1
#
#                for _ in range(4):
#                    one_cycle(brights) 
#            
#            for _ in range(8*3):
#                brights = np.ones(n_channel) * 255
#                one_cycle(brights)    
        c = np.random.random(3) * 255

        for j in range(1, 8):
            brights = np.zeros(n_channel)

            if j < 8:
                brights[3*j:3*(j+1)] = c             

            if j > 0:
                brights[3*(j-1):3*j] = c             

            for _ in range(8):
                one_cycle(brights)

        for j in range(7, 1, -1):
            brights = np.zeros(n_channel)

            if j < 8:
                brights[3*j:3*(j+1)] = c             

            if j > 0:
                brights[3*(j-1):3*j] = c             

            for _ in range(8):
                one_cycle(brights)

        c = np.random.random(3) > 0.35
        while not c.any():
            c = np.random.random(3) > 0.35

        c = np.array([c for _ in range(8)]).flatten()
        bright = np.zeros(n_channel)
        bright[c] = 1
        for b in range(0, 256, 8):
            one_cycle(bright * b)
        for b in range(256, -1, -8):
            one_cycle(bright * b)


    except KeyboardInterrupt:
        reset()
        sys.exit()
        print "Exiting"
