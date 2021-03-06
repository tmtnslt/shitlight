import shytlight
import time

# initialize threads
shytlight.init_shitlight()

# create new frame instance
test_red = shytlight.t_chitframe()
test_green = shytlight.t_chitframe()
test_blue = shytlight.t_chitframe()

# make all leds on platine 0 red
for j in range(2):
  for i in range(8):
    test_green.brightness[j][i][0] = 0x00
    test_green.brightness[j][i][1] = 0x00
    test_green.brightness[j][i][2] = 0xff
    test_red.brightness[j][i][0] = 0x00
    test_red.brightness[j][i][1] = 0x00
    test_red.brightness[j][i][2] = 0xff
    test_blue.brightness[j][i][0] = 0xff
    test_blue.brightness[j][i][1] = 0x00
    test_blue.brightness[j][i][2] = 0x00

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
