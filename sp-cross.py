import shytlight
import numpy as np
import time
from palettable import wesanderson

# Set up the geometry
N_ROWS = 5
N_LED = 8

# Initialize threads
shytlight.init_shitlight()

# Set up the colorbar
COLORS = wesanderson.Zissou_5.colors
N_COLORS = len(COLORS)


def random_color():
    """Pick a color from the colormap at random."""
    current_color = COLORS[np.random.randint(N_COLORS)]
    while np.mean(current_color) < 64:
        current_color = COLORS[np.random.randint(N_COLORS)]
    return current_color


def one_flash(current_color):
    random_row = np.random.randint(N_ROWS)
    random_led = np.random.randint(N_LED)
    brightness = np.zeros((5, 8, 3))
    brightness[random_row, random_led, :] = current_color

    offset = 0
    decreasing = False
    while not decreasing:
        decreasing = True
        rep = 1
        if random_row + offset < N_ROWS:
            brightness[random_row+offset, random_led, :] = current_color
            rep = 5
            decreasing = False
       
        if random_row - offset >= 0:
            brightness[random_row-offset, random_led, :] = current_color
            rep = 5
            decreasing = False

        if random_led + offset < N_LED:
            brightness[random_row, random_led+offset, :] = current_color
            rep = 5
            decreasing = False

        if random_led - offset >= 0:
            brightness[random_row, random_led-offset, :] = current_color
            rep = 5
            decreasing = False

        for _ in range(rep):
            shytlight.add_frame(rep=1, frame=brightness)
            brightness[brightness > 0] -= np.log10(brightness[brightness > 0])
            brightness[brightness < 10] = 0

        offset += 1

    while (brightness > 0).any():
        rep = 1
        if random_row + offset < N_ROWS and offset >= 0:
            brightness[random_row+offset, random_led, :] = current_color
            rep = 5
       
        if random_row - offset >= 0 and offset >= 0:
            brightness[random_row-offset, random_led, :] = current_color
            rep = 5

        if random_led + offset < N_LED and offset >= 0:
            brightness[random_row, random_led+offset, :] = current_color
            rep = 5

        if random_led - offset >= 0 and offset >= 0:
            brightness[random_row, random_led-offset, :] = current_color
            rep = 5

        for _ in range(rep):
            shytlight.add_frame(rep=1, frame=brightness)
            brightness[brightness > 0] -= np.log10(brightness[brightness > 0])
            brightness[brightness < 10] = 0

        offset -= 1

def main():
    current_color = random_color()
    while True:
        if np.random.random() < 0.1:
            current_color = random_color()
        
        one_flash(current_color)

if __name__ == "__main__":
    main()
