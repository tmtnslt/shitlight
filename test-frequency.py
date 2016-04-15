import RPi.GPIO as GPIO
import time
import numpy as np
import sys

# Define pins
_SER_pin   = 25   #pin 14 on the 75HC595
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

while True:
    try:
        for l in range(3):
            for j in range(8):
                brights = np.zeros(n_channel)
                brights[l+j*3] = 32 * (j + 1) - 1

                for _ in range(8):
                    for bit in range(0, 256, 8):
                        mask = brights <= bit
                        for i in range(n_channel-1, -1, -1):
                            if mask[i] == True:
                                GPIO.output(_SER_pin, GPIO.LOW)
                            else:
                                GPIO.output(_SER_pin, GPIO.HIGH)

                            GPIO.output(_SRCLK_pin, GPIO.HIGH) 
                            GPIO.output(_SRCLK_pin, GPIO.LOW) 

                        GPIO.output(_RCLK_pin, GPIO.HIGH) 
                        GPIO.output(_RCLK_pin, GPIO.LOW) 

            for j in range(8):
                brights = np.zeros(n_channel)
                brights[l+j*3] = 32 * (j + 1) - 1
                brights[((l+1)%3)+j*3] = 32 * (j+1) - 1

                for _ in range(8):
                    for bit in range(0, 256, 8):
                        mask = brights <= bit
                        for i in range(n_channel-1, -1, -1):
                            if mask[i] == True:
                                GPIO.output(_SER_pin, GPIO.LOW)
                            else:
                                GPIO.output(_SER_pin, GPIO.HIGH)

                            GPIO.output(_SRCLK_pin, GPIO.HIGH) 
                            GPIO.output(_SRCLK_pin, GPIO.LOW) 

                        GPIO.output(_RCLK_pin, GPIO.HIGH) 
                        GPIO.output(_RCLK_pin, GPIO.LOW) 


    except KeyboardInterrupt:
        reset()
        sys.exit()
        print "Exiting"
