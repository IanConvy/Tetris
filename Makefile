objects = obj/main.o obj/drawer.o obj/shader.o obj/text.o obj/stb_image.o \
	obj/board.o obj/pieces.o obj/grid.o obj/inputs.o obj/nes.o \
	obj/pointclick.o obj/evaluate.o obj/move.o obj/glad.o

tetris : $(objects)
	g++ -Iinclude `pkg-config --cflags glfw3` $(objects) -o tetris -lGL `pkg-config --libs --static glfw3`

obj/main.o : src/game/main.cpp include/game/inputs.hpp include/game/nes.hpp include/game/pointclick.hpp
	mkdir -p obj
	g++ -Iinclude -c src/game/main.cpp -o obj/main.o

obj/drawer.o : src/graphics/drawer.cpp include/graphics/stb_image.hpp include/graphics/shader.hpp \
	include/graphics/text.hpp include/graphics/drawer.hpp include/game/pieces.hpp include/game/grid.hpp
	g++ -Iinclude -c src/graphics/drawer.cpp -o obj/drawer.o

obj/shader.o : src/graphics/shader.cpp include/graphics/shader.hpp
	g++ -Iinclude -c src/graphics/shader.cpp -o obj/shader.o

obj/text.o : src/graphics/text.cpp include/graphics/text.hpp
	g++ -Iinclude -c src/graphics/text.cpp -o obj/text.o

obj/stb_image.o : src/graphics/stb_image.cpp include/graphics/stb_image.hpp
	g++ -Iinclude -c src/graphics/stb_image.cpp -o obj/stb_image.o

obj/board.o : src/game/board.cpp include/game/board.hpp include/game/pieces.hpp include/game/grid.hpp
	g++ -Iinclude -c src/game/board.cpp -o obj/board.o

obj/pieces.o : src/game/pieces.cpp include/game/pieces.hpp
	g++ -Iinclude -c src/game/pieces.cpp -o obj/pieces.o

obj/grid.o : src/game/grid.cpp include/game/grid.hpp
	g++ -Iinclude -c src/game/grid.cpp -o obj/grid.o

obj/inputs.o : src/game/inputs.cpp include/game/inputs.hpp
	g++ -Iinclude -c src/game/inputs.cpp -o obj/inputs.o

obj/nes.o : src/game/nes.cpp include/game/nes.hpp include/game/pieces.hpp include/game/grid.hpp
	g++ -Iinclude -c src/game/nes.cpp -o obj/nes.o

obj/pointclick.o : src/game/pointclick.cpp include/game/pointclick.hpp include/game/board.hpp \
	include/game/pieces.hpp include/game/grid.hpp include/ai/evaluate.hpp
	g++ -Iinclude -c src/game/pointclick.cpp -o obj/pointclick.o

obj/evaluate.o : src/ai/evaluate.cpp include/ai/evaluate.hpp include/game/board.hpp \
	include/game/pieces.hpp include/game/grid.hpp include/ai/move.hpp
	g++ -Iinclude -c src/ai/evaluate.cpp -o obj/evaluate.o

obj/move.o : src/ai/move.cpp include/ai/move.hpp include/game/grid.hpp include/game/board.hpp
	g++ -Iinclude -c src/ai/move.cpp -o obj/move.o

obj/glad.o : src/glad/glad.c include/glad/glad.h
	g++ -Iinclude -c src/glad/glad.c -o obj/glad.o









