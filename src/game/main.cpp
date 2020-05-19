#include <string>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "graphics/drawer.hpp"
#include "game/inputs.hpp"
#include "game/nes.hpp"
#include "game/pointclick.hpp"

int main(int argc, char* argv[])
{
    // Initialize GLFW, set the minimum version at 3.3, and use the core OpenGL profile
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    { // This scope holds all of the OpenGL and GLFW operations

        // Create the game window and assign to it an OpenGL context loaded by GLEW
        int windowHeight = 899, windowWidth = 1035; // Set initial dimensions, but will change if manually resized
        GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Tetris", nullptr, nullptr);
        glfwMakeContextCurrent(window);
        gladLoadGL(); // GLAD loads the appropriate OpenGL functions and variables

        // Extract image/shader parent directory, game type, and level from command line
        const std::string drawingLocation = argv[1];
        const std::string mode = (argc > 2) ? argv[2] : std::string("nes");
        const int startLevel = (argc > 3) ? std::stoi(argv[3]) : 0;

        // Create the keyboard/mouse input handler and the OpenGL drawer
        InputHandler inputs{window};
        BoardDrawer drawer{drawingLocation};

        // Initialize the specified game mode and begin the frame loop
        if (mode == std::string("nes")) {

            // Create game and assign its display variables to the drawer
            NESTetris game{startLevel};
            game.assignInput(inputs);
            drawer.assignGrid(game.displayGrid);
            drawer.assignLevel(game.dynamic["level"]);
            drawer.assignLineCount(game.board.lineCount);
            drawer.assignlineTypeCount(game.board.lineTypeCount);
            drawer.assignNextPiece(game.nextPiece);
            drawer.assignScore(game.dynamic["score"]);

            /* 
             * Set the engine time and the rendering time. Engine time
             * is used to tell the game how fast to process frames, which
             * determines the maximum speed of play and the overall timing 
             * resolution. Rendering time sets how rapidy the OpenGL display 
             * should be refreshed. The two times are decoupled to allow the game 
             * to be played at high speed or precision without the CPU overhead
             * of rapidly writing to the display buffer, which would be wasteful.
             */
            double engTime = 0;
            double rendTime = 0;
            const double engSecs = 1 / 60.1; // Recipricol of FPS
            const double rendSecs = 1 / 60.1; // Recipricol of FPS
            
            // Run the main game loop, with game frames run and drawn seperately
            while (!glfwWindowShouldClose(window)) {
                double newTime = glfwGetTime();
                if (newTime - engTime >= engSecs) {
                    game.runFrame();
                    engTime = newTime;  
                }
                if (newTime - rendTime >= rendSecs) {
                    drawer.drawFrame();
                    glfwSwapBuffers(window);
                    rendTime = newTime;
                }
                /* 
                 * We have GLFW detect events at an unconstrained speed to 
                 * maximize responsiveness and prevent events from piling up 
                 */
                glfwPollEvents();
            }
        }
        else if (mode == std::string("pointclick")) {

            // Create game and assign its display variables to the drawer
            PointClick game{startLevel};
            game.assignInput(inputs);
            drawer.assignGrid(game.displayGrid);
            drawer.assignLevel(game.dynamic["level"]);
            drawer.assignLineCount(game.board.lineCount);
            drawer.assignlineTypeCount(game.board.lineTypeCount);
            drawer.assignNextPiece(game.nextPiece);
            drawer.assignScore(game.dynamic["score"]);
            drawer.assignMiscData(game.eval);

            /*
             * The point-and-click mode does not have an internal or "engine"
             * frame rate, so the only timing to track is the display refresh
             * rate.
             */
            double rendTime = 0;
            const double rendSecs = 1 / 60.1;
            
            while (!glfwWindowShouldClose(window)) {
                double newTime = glfwGetTime();
                if (newTime - rendTime >= rendSecs) {
                    game.runFrame();
                    drawer.drawFrame();
                    glfwSwapBuffers(window);
                    rendTime = newTime;
                }
                glfwPollEvents();
            }
        }
    }    
    glfwTerminate();
    return 0;
}