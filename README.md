# Tetris Emulation with OpenGL

This application precisely emulates the 1989 release of Tetris for the Nintendo Entertainment System using OpenGL. The codebase was written from scratch in C++, and can be compiled on Linux by following the instructions below. A general description of the emulator and its implementation can be found on [my website](https://ianconvy.github.io/projects/tetris/tetris-emulator/tetris-emulator.html).

## Building from Source

Steps needed to build the emulator from source on Ubuntu (requires OpenGL 3.3 or later):

1. Download GLFW: https://www.glfw.org/download.html

2. Download CMake and xorg-dev for GLFW:

		$ sudo apt install cmake xorg-dev

3. Prepare a build for GLFW by running CMake in the extracted GLFW directory:

		$ cmake .

4. Build and install GLFW by running Make in the GLFW directory

		$ make
		$ sudo make install
		$ make clean

5. Create a symbolic link in /usr/lib to the OpenGL library (if not already present):
          
          $ sudo ln -s /usr/lib/x86_64-linux-gnu/libGL.so.1 /usr/lib/libGL.so

6. Run Make in the Tetris directory to compile

		$ make

7. To run the game you need to specify the location of the assets directory in the first 
   argument, and can optionally specify the game type "nes" or "pointclick" in the second 
   argument (defaults to "nes"), and the starting level in the third argument (defaults 
   to 0). For example, to run Tetris in NES mode on level 18 while in the source directory:

		$ ./tetris assets nes 18


## Game Controls

Controls for NES mode:

                left arrow : move piece left
               right arrow : move piece right
                down arrow : soft drop
                         a : rotate piece counterclockwise
                         s : rotate piece clockwise
                    escape : reset game 

Controls for point-and-click mode:

              mouse cursor : moves piece
         left mouse button : place piece
        right mouse button : rotate piece clockwise
                         a : rotate piece counterclockwise
                         s : rotate piece clockwise
                         z : go back one move
                         x : go forward one move
                    escape : reset game
