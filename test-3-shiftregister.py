import RPi.GPIO as GPIO
import time
import numpy as np

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False) 

def clock():
    for pin in range(10, 15):
        GPIO.output(pin, GPIO.HIGH) 
        GPIO.output(pin, GPIO.LOW) 

def latch():
    for pin in range(5, 10):
        GPIO.output(pin, GPIO.HIGH) 
        GPIO.output(pin, GPIO.LOW) 

def setup():
    for pin in range(15):
        GPIO.setup(pin, GPIO.OUT)
        GPIO.setup(pin, GPIO.OUT)
        GPIO.setup(pin, GPIO.OUT)

def reset():
    for pin in range(15):
        GPIO.output(pin, GPIO.LOW)
        GPIO.output(pin, GPIO.LOW)
        GPIO.output(pin, GPIO.LOW)

def low():
    for pin in range(5):
        GPIO.output(pin, GPIO.LOW)

def high():
    for pin in range(5):
        GPIO.output(pin, GPIO.HIGH)

def clean():
    low()
    print("Reset all LED")
    for _ in range(24):
        clock()
    latch()

# Initial setup()
setup()
reset()
clean()

try:
    while True:
        # Enter a single 1
        high()
        clock()
        latch()
        low()
        for _ in range(24):
            time.sleep(0.15)
            print _
            clock()
            latch()
except:
    clean()
    reset()
