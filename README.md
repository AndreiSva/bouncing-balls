# bouncing-balls
This is some code I was experimenting with some time ago and just decided to open source. it can handle around 10,000 balls at 60 fps on my computer.

# Building
Obviously you need the sdl2, sdl2_ttf and sdl2_image libraries which should be installable from your distribution's package manager. Afterwards you can run the following commands:

on Linux: `cc balls.c -lSDL2 -lSDL2_ttf -lSDL2_image -o balls`
on OpenBSD: `cc balls.c -I/usr/local/include/ -L/usr/local/lib/ -lSDL2 -lSDL2_ttf -lSDL2_image -o balls`

then you can run it with `./balls`

# Controls

| key  | description |
| ------------- | ------------- |
| F  | Toggle Framerate  |
| C | Toggle color  |
| R-L arrow keys | Increase/decrease target fps |
| U-D arrow keys| Increase/decrease elasticity  |
| V | Toggle Vsync |
| Q | Reset balls |
| R | Toggle random ball sizes |

Clicking anywhere spawns a new ball.

# Bugs

- The code is not exactly the cleanest.

- Q does not reset the balls position.
