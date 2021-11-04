# LEDMatrix

An arduino neopixel display program. Couldn't find any nice pretty graphics to put on it so I made these. 

You'll need to install the neopixel matrix library.

They are roughly based on some glsl shader code, using a nested for loop to assign each pixel a colour based on its pixel coord. Should resize nicely to any size matrix, as shader code maps the screen size from 0-1. (although could be a bit computationally expensive for a bigger matrix.

