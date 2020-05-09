Steps needed to build Tetris program from scratch on Ubuntu:

1. Download GLEW: http://glew.sourceforge.net/

2. Download the following tools needed to build GLEW (taken from GLEW readme):
	$ sudo apt install build-essential libxmu-dev libxi-dev libgl-dev libosmesa-dev

3. Build and install GLEW by running Make in the extracted GLEW directory:
	$ make
	$ sudo make install
	$ make clean

4. To make life easier, comment out the following lines in glew.h. This prevents GLEW from trying to 
   include the missing and deprecated glu.h header file that the IDE/compiler will complain about:

	ifndef GLEW_NO_GLU
	/* this is where we can safely include GLU */
	  if defined(__APPLE__) && defined(__MACH__)
	    include <OpenGL/glu.h>
	  else
	    include <GL/glu.h>
	  endif
	endif

5. The GLEW library files may be placed in /usr/lib64, which the compiler and/or executable may not know 
   to look in by default (when using GLEW as a shared library it failed at runtime). An easy solution is 
   to just copy the library to /usr/lib (vs settting compiler flags and runtime variables):
	$ sudo cp /usr/lib64/libGLEW.so.2.1 /usr/lib

6. Download GLFW: https://www.glfw.org/download.html

7. Download CMake and xorg-dev for GLFW:
	$ sudo apt install cmake xorg-dev

8. Prepare a build for GLFW by running CMake in the extracted GLFW directory:
	$ cmake .

9. Build and install GLFW by running Make in the GLFW directory
	$ make
	$ sudo make install
	$ make clean

10. Run Make in the Tetris directory to compile 
	$ make

11. To run the game, you need to specify the location of the shaders/ and images/ directories in the first argument, the 
    the game type in the second argument (nes or pointclick), and the starting level in the third argument
	$ ./tetris <parent dir> <game type> <level>

