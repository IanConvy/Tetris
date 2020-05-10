objects = objects/main.o objects/drawer.o objects/shader.o objects/text.o objects/stb_image.o \
	objects/board.o objects/pieces.o objects/grid.o objects/inputs.o objects/nes.o \
	objects/pointclick.o objects/evaluate.o objects/move.o

tetris : $(objects)
	g++ `pkg-config --cflags glfw3` $(objects) -o tetris -lGL -lGLEW `pkg-config --libs --static glfw3`

objects/main.o : game/main.cpp game/headers/inputs.hpp game/headers/nes.hpp game/headers/pointclick.hpp
	mkdir -p objects
	g++ -c game/main.cpp -o objects/main.o

objects/drawer.o : graphics/drawer.cpp graphics/headers/stb_image.hpp graphics/headers/shader.hpp \
	graphics/headers/text.hpp graphics/headers/drawer.hpp game/headers/pieces.hpp game/headers/grid.hpp
	g++ -c graphics/drawer.cpp -o objects/drawer.o

objects/shader.o : graphics/shader.cpp graphics/headers/shader.hpp
	g++ -c graphics/shader.cpp -o objects/shader.o

objects/text.o : graphics/text.cpp graphics/headers/text.hpp
	g++ -c graphics/text.cpp -o objects/text.o

objects/stb_image.o : graphics/stb_image.cpp graphics/headers/stb_image.hpp
	g++ -c graphics/stb_image.cpp -o objects/stb_image.o

objects/board.o : game/board.cpp game/headers/board.hpp game/headers/pieces.hpp game/headers/grid.hpp
	g++ -c game/board.cpp -o objects/board.o

objects/pieces.o : game/pieces.cpp game/headers/pieces.hpp
	g++ -c game/pieces.cpp -o objects/pieces.o

objects/grid.o : game/grid.cpp game/headers/grid.hpp
	g++ -c game/grid.cpp -o objects/grid.o

objects/inputs.o : game/inputs.cpp game/headers/inputs.hpp
	g++ -c game/inputs.cpp -o objects/inputs.o

objects/nes.o : game/nes.cpp game/headers/nes.hpp game/headers/pieces.hpp game/headers/grid.hpp
	g++ -c game/nes.cpp -o objects/nes.o

objects/pointclick.o : game/pointclick.cpp game/headers/pointclick.hpp game/headers/board.hpp \
	game/headers/pieces.hpp game/headers/grid.hpp ai/headers/evaluate.hpp
	g++ -c game/pointclick.cpp -o objects/pointclick.o

objects/evaluate.o : ai/evaluate.cpp ai/headers/evaluate.hpp game/headers/board.hpp \
	game/headers/pieces.hpp game/headers/grid.hpp ai/headers/move.hpp
	g++ -c ai/evaluate.cpp -o objects/evaluate.o

objects/move.o : ai/move.cpp ai/headers/move.hpp game/headers/grid.hpp game/headers/board.hpp
	g++ -c ai/move.cpp -o objects/move.o









