objects = objects/main.o objects/drawer.o objects/shader.o objects/text.o objects/stb_image.o \
	objects/board.o objects/pieces.o objects/grid.o objects/inputs.o objects/nes.o \
	objects/pointclick.o objects/evaluate.o objects/move.o objects/player.o

tetris : $(objects)
	g++ `pkg-config --cflags glfw3` $(objects) -o tetris -lGL -lGLEW `pkg-config --libs --static glfw3`

objects/main.o : game/main.cpp
	mkdir -p objects
	g++ -c game/main.cpp -o objects/main.o

objects/drawer.o : graphics/drawer.cpp
	g++ -c graphics/drawer.cpp -o objects/drawer.o

objects/shader.o : graphics/shader.cpp
	g++ -c graphics/shader.cpp -o objects/shader.o

objects/text.o : graphics/text.cpp
	g++ -c graphics/text.cpp -o objects/text.o

objects/stb_image.o : graphics/stb_image.cpp
	g++ -c graphics/stb_image.cpp -o objects/stb_image.o

objects/board.o : game/board.cpp
	g++ -c game/board.cpp -o objects/board.o

objects/pieces.o : game/pieces.cpp
	g++ -c game/pieces.cpp -o objects/pieces.o

objects/grid.o : game/grid.cpp
	g++ -c game/grid.cpp -o objects/grid.o

objects/inputs.o : game/inputs.cpp
	g++ -c game/inputs.cpp -o objects/inputs.o

objects/nes.o : game/nes.cpp
	g++ -c game/nes.cpp -o objects/nes.o

objects/pointclick.o : game/pointclick.cpp
	g++ -c game/pointclick.cpp -o objects/pointclick.o

objects/evaluate.o : ai/evaluate.cpp
	g++ -c ai/evaluate.cpp -o objects/evaluate.o

objects/move.o : ai/move.cpp
	g++ -c ai/move.cpp -o objects/move.o

objects/player.o : ai/player.cpp
	g++ -c ai/player.cpp -o objects/player.o











