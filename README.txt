Steps needed to build Tetris program from scratch on Ubuntu (requires OpenGL 3.3 or later):

1. Download GLFW: https://www.glfw.org/download.html

2. Download CMake and xorg-dev for GLFW:

		$ sudo apt install cmake xorg-dev

3. Prepare a build for GLFW by running CMake in the extracted GLFW directory:

		$ cmake .

4. Build and install GLFW by running Make in the GLFW directory

		$ make
		$ sudo make install
		$ make clean

5. Run Make in the Tetris directory to compile

		$ make

6. To run the game you need to specify the location of the assets/ directory in the first 
	argument, and can optionally specify the game type "nes" or "pointclick" in the second 
	argument (defaults to "nes"), and the starting level in the third argument (defaults to 0)

		$ ./tetris <assets dir> <game type> <level>

Controls for NES mode:

		 		left arrow : move piece left
			   right arrow : move piece right
		 		down arrow : soft drop
		          		 a : rotate piece counterclockwise
		          		 s : rotate piece clockwise
		     		escape : reset game 

Controls for Point-and-Click mode:

	          mouse cursor : moves piece
         left mouse button : place piece
        right mouse button : rotate piece clockwise
		          		 a : rotate piece counterclockwise
		          		 s : rotate piece clockwise
		         		 z : go back one move
		          		 s : go forward one move
		     		escape : reset game