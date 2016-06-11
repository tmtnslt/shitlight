import shytlight
import time

# initialize threads
shytlight.init_shitlight()

# create new frame instance
test_red = shytlight.t_chitframe()
test_green = shytlight.t_chitframe()
test_blue = shytlight.t_chitframe()

# make all leds on platine 0 red
for i in range(8):
    test_green.brightness[0][i][0] = 0x2f
    test_green.brightness[0][i][1] = 0x41
    test_green.brightness[0][i][2] = 0x93
    test_red.brightness[0][i][0] = 0x4f
    test_red.brightness[0][i][1] = 0x2f
    test_red.brightness[0][i][2] = 0x93
    test_blue.brightness[0][i][0] = 0x2f
    test_blue.brightness[0][i][1] = 0x74
    test_blue.brightness[0][i][2] = 0x93

# add frame to thread
shytlight.add_frame(200, test_green) 
shytlight.add_frame(200, test_red) 
shytlight.add_frame(200, test_blue) 
shytlight.add_frame(200, test_green) 
shytlight.add_frame(200, test_red) 
shytlight.add_frame(200, test_blue) 
shytlight.add_frame(200, test_green) 
shytlight.add_frame(200, test_red) 
shytlight.add_frame(200, test_blue) 
shytlight.add_frame(200, test_green) 
shytlight.add_frame(200, test_red) 
shytlight.add_frame(200, test_blue) 
shytlight.add_frame(200, test_green) 
shytlight.add_frame(200, test_red) 
shytlight.add_frame(200, test_blue) 
shytlight.add_frame(200, test_green) 
shytlight.add_frame(200, test_red) 
shytlight.add_frame(200, test_blue) 
# now sleep, otherwise threads will finish
time.sleep(1)
print(shytlight.get_fps())
time.sleep(1)
print(shytlight.get_fps())
time.sleep(1000)
print(shytlight.get_fps())
