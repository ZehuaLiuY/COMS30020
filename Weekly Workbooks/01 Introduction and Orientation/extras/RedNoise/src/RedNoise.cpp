#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <vector>
#include "glm/vec3.hpp"
#include <CanvasPoint.h>
#include <Colour.h>
#include "interpolation.h"
#include "triangle.h"
#include "wireframes.h"
#include "ray.h"

#define WIDTH 320
#define HEIGHT 240

bool orbitActivated = false;
bool rayTracingActivated = false;

void draw(DrawingWindow &window, glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation) {
    // window.clearPixels();
    // drawRGBColour(window);
    if(orbitActivated){
        orbitClockwise(window,cameraPosition, cameraOrientation, 0.005);
    }
    renderWireframe(window, cameraPosition, cameraOrientation);
}

void handleEvent(SDL_Event event, DrawingWindow &window, glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation) {
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_LEFT) {
            // make the camera move left
            std::cout << "Camera moving LEFT" << std::endl;
            window.clearPixels();
            resetDepthBuffer();
            translateCamera(cameraPosition, -0.1, 0, 0);
            // std::cout << cameraPosition.x << " " << cameraPosition.y << " " << cameraPosition.z << std::endl;
        }
        else if (event.key.keysym.sym == SDLK_RIGHT) {
            // make the camera move right
            std::cout << "Camera moving RIGHT" << std::endl;
            window.clearPixels();
            resetDepthBuffer();
            translateCamera(cameraPosition, 0.1, 0, 0);
        }
        else if (event.key.keysym.sym == SDLK_UP) {
            // make the camera move up
            std::cout << "Camera moving UP" << std::endl;
            window.clearPixels();
            resetDepthBuffer();
            translateCamera(cameraPosition, 0, 0.1, 0);
        }
        else if (event.key.keysym.sym == SDLK_DOWN) {
            // make the camera move down
            std::cout << "Camera moving DOWN" << std::endl;
            window.clearPixels();
            resetDepthBuffer();
            translateCamera(cameraPosition, 0, -0.1, 0);

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
            Colour fillColour(rand() % 256, rand() % 256, rand() % 256);
            drawFilledTriangle(window, triangle, fillColour);

            // Draw a stroked triangle over the filled triangle
            Colour strokeColour(255, 255, 255); // white frame
            drawStrokedTriangle(window, triangle, strokeColour);

            window.renderFrame();
        }
        else if (event.key.keysym.sym == SDLK_t) {
            testTexturedTriangle(window);
            window.renderFrame();
        }

        // rotation matrix x
        else if (event.key.keysym.sym == SDLK_q) {
            window.clearPixels();
            resetDepthBuffer();
            rotateCameraByX(cameraPosition);
        }
        else if (event.key.keysym.sym == SDLK_w) {
            window.clearPixels();
            resetDepthBuffer();
            rotateUp(cameraPosition, cameraOrientation, 0.01);
        }
        // rotation matrix y
        else if (event.key.keysym.sym == SDLK_a) {
            window.clearPixels();
            resetDepthBuffer();
            rotateCameraByY(cameraPosition);
        }
        else if (event.key.keysym.sym == SDLK_s) {
            window.clearPixels();
            resetDepthBuffer();
            rotateClock(cameraPosition, cameraOrientation, 0.01);

        } else if (event.key.keysym.sym == SDLK_l) {
            std::cout << "Before: \n" << std::endl;
            for (size_t i = 0; i < 3; i++){
                for (size_t j = 0; j < 3; j++)
                {
                    std::cout << cameraOrientation[i][j] << " , ";
                }
                std::cout << "\n";
            }
            std::cout << "\n";

            lookAt(cameraPosition,cameraOrientation);

            std::cout << "After: \n" << std::endl;
            for (size_t i = 0; i < 3; i++){
                for (size_t j = 0; j < 3; j++)
                {
                    std::cout << cameraOrientation[i][j] << " , ";
                }
                std::cout << "\n";
            }
        }
        else if (event.key.keysym.sym == SDLK_o) {
            window.clearPixels();
            orbitActivated = !orbitActivated;
        }
        else if (event.key.keysym.sym == SDLK_r){
            window.clearPixels();
            rayTracingActivated = !rayTracingActivated;
        }

    } else if (event.type == SDL_MOUSEBUTTONDOWN) {
        window.savePPM("output.ppm");
        window.saveBMP("output.bmp");
    }
}

int main(int argc, char *argv[]) {
    DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
    SDL_Event event;

    glm::mat3 cameraOrientation = glm::mat3(
            glm::vec3(1, 0, 0),  // right
            glm::vec3(0, 1, 0),  // up
            glm::vec3(0, 0, 1)  // forward
    );

    glm::vec3 cameraPosition = glm::vec3 (0.0, 0.0, 4.0);

    std::vector<ModelTriangle> modelTriangles = readFiles("../src/files/cornell-box.obj", "../src/files/cornell-box.mtl", 0.35);
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

    // test for ray tracing

    while (true) {
        // We MUST poll for events - otherwise the window will freeze !
        if (window.pollForInputEvents(event)) handleEvent(event, window, cameraPosition, cameraOrientation);
        // for orbit pause and resume
        window.clearPixels();
        resetDepthBuffer();
        if (rayTracingActivated) {
            drawRayTracedScene(window, cameraPosition, 2.0, modelTriangles);
        }
        else{
            draw(window, cameraPosition, cameraOrientation);
        }

        // Need to render the frame at the end, or nothing actually gets shown on the screen !
        window.renderFrame();
    }
}