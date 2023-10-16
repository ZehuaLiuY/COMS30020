#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <vector>
#include "glm/vec3.hpp"
#include <CanvasPoint.h>
#include <Colour.h>
#include "week2.h"
#include "week3.h"
#include "week4.h"

#define WIDTH 320
#define HEIGHT 240


void draw(DrawingWindow &window) {
    //window.clearPixels();
    // drawRGBColour(window);
}

void handleEvent(SDL_Event event, DrawingWindow &window) {
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_LEFT) std::cout << "LEFT" << std::endl;
        else if (event.key.keysym.sym == SDLK_RIGHT) std::cout << "RIGHT" << std::endl;
        else if (event.key.keysym.sym == SDLK_UP) std::cout << "UP" << std::endl;
        else if (event.key.keysym.sym == SDLK_DOWN) {
            std::vector<ModelTriangle> modelTriangles = readFiles("../src/files/cornell-box.obj", "../src/files/cornell-box.mtl", 0.35);
            for (const ModelTriangle& modelTriangle : modelTriangles) {
                std::cout << modelTriangle << std::endl;
            }

        }
        else if (event.key.keysym.sym == SDLK_u) {

            // Generate random vertices for the triangle
            CanvasTriangle triangle = generateRandomTriangle(window);


            Colour color(rand() % 256, rand() % 256, rand() % 256);

            drawStrokedTriangle(window, triangle, color);

            window.renderFrame();
        }
        else if (event.key.keysym.sym == SDLK_f) {
            // Generate random vertices for the filled triangle
            CanvasTriangle triangle = generateRandomTriangle(window);

            // Generate a random color for filling
            Colour fillColor(rand() % 256, rand() % 256, rand() % 256);
            drawFilledTriangle(window, triangle, fillColor);

            // Draw a stroked triangle over the filled triangle
            Colour strokeColor(255, 255, 255); // white frame
            drawStrokedTriangle(window, triangle, strokeColor);

            window.renderFrame();
        }
        else if (event.key.keysym.sym == SDLK_t) {
            testTexturedTriangle(window);
            window.renderFrame();
        }
    } else if (event.type == SDL_MOUSEBUTTONDOWN) {
        window.savePPM("output.ppm");
        window.saveBMP("output.bmp");
    }
}

int main(int argc, char *argv[]) {
    DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
    SDL_Event event;

//    // test for interpolateSingleFloats
//    std::vector<float> result;
//    result = interpolateSingleFloats(2.2, 8.5, 7);
//    for(size_t i=0; i<result.size(); i++) std::cout << result[i] << " ";
//    std::cout << std::endl;
//
//    //test for interpolateThreeElementValues
//    glm::vec3 from(1.0f, 4.0f, 9.2f);
//    glm::vec3 to(4.0f, 1.0f, 9.8f);
//    int numberOfValues = 4; // Adjust the number of values as needed
//
//    std::vector<glm::vec3> interpolatedValues = interpolateThreeElementValues(from, to, numberOfValues);
//
//    for (const glm::vec3& value : interpolatedValues) {
//        std::cout << "(" << value.x << ", " << value.y << ", " << value.z << ") ";
//    }
//    std::cout << std::endl;

    while (true) {
        // We MUST poll for events - otherwise the window will freeze !
        if (window.pollForInputEvents(event)) handleEvent(event, window);
        draw(window);
        // Need to render the frame at the end, or nothing actually gets shown on the screen !
        window.renderFrame();
    }
}