import shiftpi
import time
import numpy as np

class LED():

    def __init__(self, pin_number):

        self._is_active = False
        self._pin = pin_number
        self._getting_brighter = False
        self._getting_darker = False
        self._current_step = 0
        self._current_total_steps = 0

    def active(self):

        return self._is_active

    def one_flash(self, on, off):
        shiftpi.digitalWrite(self._pin, shiftpi.HIGH) 
        time.sleep(on)
        shiftpi.digitalWrite(self._pin, shiftpi.LOW) 
        time.sleep(off)
      
    def start_one_cycle(self, duration):

        self._current_total_steps = duration * 500.
        self._current_step = 0

        self._is_active = True
        self._getting_brighter = True
        self._getting_darker = False

    def one_step(self):
        one_cycle = 1 / 500.
        on = (1. * self._current_step / self._current_total_steps) * one_cycle
        off = (1 - on) * one_cycle
        
        if self._getting_brighter:
            self._current_step = self._current_step + 1
        else:
            self._current_step = self._current_step - 1

        if self._current_step == self._current_total_steps:
            self._getting_brighter = False
            self._getting_darker = True
        
        if self._current_step == 0:
            self._is_active = False

        return on, off 


    def pin(self):
        return self._pin

#    def one_cycle(self, duration, fps=100):
#
#        self._is_active = True
#        
#        shiftpi.digitalWrite(self._pin, shiftpi.LOW)
#        
#        steps = int(0.5 * duration * fps)
#        one_cycle = 1. / fps
#        t0 = time.time()
#
#        for s in range(1, steps):
#            on = (1. * s / steps) * one_cycle
#            off = (1 - on) * one_cycle
#            self.one_flash(on, off)
#
#        for s in range(steps, 0, -1):
#            on = (1. * s / steps) * one_cycle
#            off = (1 - on) * one_cycle
#            self.one_flash(on, off)
#
#        shiftpi.digitalWrite(self._pin, shiftpi.LOW)
#        
#        self._is_active = False
#
