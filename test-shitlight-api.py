import shytlight
import time

# initialize threads
shytlight.init_shitlight()

# create new frame instance
test_red = shytlight.t_chitframe()

# make all leds on platine 0 red
for i in range(8):
    test_red.brightness[0][i][0] = 1024

# add frame to thread
shytlight.add_frame(1000, test_red) 

# now sleep, otherwise threads will finish
time.sleep(10)

