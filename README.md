# Project1_Graphics

This project implements an interactive 3D cubic grid using OpenGL.
The program creates a 5 × 5 × 5 cubic grid with a movable 1 × 1 × 1 solid cube inside it. The cube can be moved along the X, Y, and Z axes, its color can be changed using RGB values, and individual grid cells can be filled or cleared.
The complete grid, including filled cells and the movable cube, can also be rotated interactively.

## Features

- 5 × 5 × 5 3D cubic grid
- 1 × 1 × 1 movable cube
- Movement along X, Y and Z axes
- User-defined RGB cube color
- Fill and clear individual grid cells
- Rotate the complete grid in four directions
- Cube remains within the grid boundaries

## Controls

Key	        Action
← / →	    Move cube along X-axis
↑ / ↓	    Move cube along Y-axis
U / B	    Move cube along Z-axis
C	        Change cube color
F	        Fill current grid cell
W	        Clear current grid cell
L / R       Rotate grid left/right
T / D	    Rotate grid up/down
ESC	        Exit


When C is pressed, enter the RGB values in the terminal using values between `0.0` and `1.0`.

To run the code -> make run will compile and run with the help of Makefile