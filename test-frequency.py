import RPi.GPIO as GPIO
import time
import numpy as np

# Define pins
_SER_pin   = 25   #pin 14 on the 75HC595
_RCLK_pin  = 24   #pin 12 on the 75HC595
_SRCLK_pin = 23   #pin 11 on the 75HC595

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False) 

GPIO.setup(_SER_pin, GPIO.OUT)
GPIO.setup(_RCLK_pin, GPIO.OUT)
GPIO.setup(_SRCLK_pin, GPIO.OUT)

times = []
#brights = [0, 0, 0, 0 ,0, 0, 0, 0, 0, 0, 200, 0, 0, 0, 0, 0]
brights = np.ones(16) * 64
brights = np.array(brights)
n_channel = len(brights)

# Reset all LED
for i in range(n_channel-1, -1, -1):
    GPIO.output(_SER_pin, GPIO.LOW)
    GPIO.output(_SRCLK_pin, GPIO.HIGH) 
    GPIO.output(_SRCLK_pin, GPIO.LOW) 
GPIO.output(_RCLK_pin, GPIO.HIGH) 
GPIO.output(_RCLK_pin, GPIO.LOW) 

time.sleep(0.25)
#GPIO.output(_SER_pin, GPIO.LOW)
##GPIO.output(_SRCLK_pin, GPIO.HIGH) 
##GPIO.output(_SRCLK_pin, GPIO.LOW) 
#    
#GPIO.output(_SER_pin, GPIO.HIGH)
#GPIO.output(_SRCLK_pin, GPIO.HIGH) 
#GPIO.output(_SRCLK_pin, GPIO.LOW) 
#GPIO.output(_RCLK_pin, GPIO.HIGH) 
#GPIO.output(_RCLK_pin, GPIO.LOW) 
#time.sleep(0.25)
#GPIO.output(_SER_pin, GPIO.LOW)
#
#for _ in range(15):
#    GPIO.output(_SRCLK_pin, GPIO.HIGH) 
#    GPIO.output(_SRCLK_pin, GPIO.LOW) 
#    GPIO.output(_RCLK_pin, GPIO.HIGH) 
#    GPIO.output(_RCLK_pin, GPIO.LOW) 
#    time.sleep(0.25)
#
##GPIO.output(_RCLK_pin, GPIO.LOW) 
#print n_channel
#
#GPIO.output(_SRCLK_pin, GPIO.LOW) 
#GPIO.output(_SER_pin, GPIO.HIGH)

while True:
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
        
print np.mean(times)
