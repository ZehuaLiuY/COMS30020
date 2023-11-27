#include "interpolation.h"
#include "triangle.h"
#include "wireframes.h"
#include "ray.h"
#include "sphere.h"

#define WIDTH 640
#define HEIGHT 480

bool orbitActivated = false;

enum class RenderingMode { Wireframe, Rasterised, RayTraced, Flat, SphereGouraud, SpherePhong };
RenderingMode currentMode = RenderingMode::Rasterised;

void draw(DrawingWindow &window, glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation,
          std::vector<ModelTriangle> &modelTriangles, std::vector<ModelTriangle> &sphereTriangles, glm::vec3 &lightPosition) {

    std::vector<std::pair<CanvasTriangle, Colour>> triangles = triangleTransformer(modelTriangles, cameraPosition, cameraOrientation);
    glm::vec3 sphereCamPos = glm::vec3(0.0, 0.0, 4.0);
    glm::vec3 sphereLightPos = glm::vec3(0.5, 0.8, 1.6);
    std::vector<std::pair<CanvasTriangle, Colour>> sTriangles = triangleTransformer(sphereTriangles, sphereCamPos, cameraOrientation);


    if(orbitActivated){
        orbitClockwise(cameraPosition, cameraOrientation, 0.001);
    }

    window.clearPixels();

    switch (currentMode) {
        case RenderingMode::Wireframe:
            resetDepthBuffer();
            renderWireframe(window, triangles);
            break;
        case RenderingMode::Rasterised:
            resetDepthBuffer();
            renderRasterised(window,  triangles);
            break;
        case RenderingMode::RayTraced:
            resetDepthBuffer();
            drawRayTracedScene(window, cameraPosition, cameraOrientation, 2.0, modelTriangles, lightPosition);
            break;
        case RenderingMode::Flat:
            resetDepthBuffer();
            flatShading(window, sphereCamPos, cameraOrientation, sphereLightPos, 2.0, sphereTriangles);
            break;
        case RenderingMode::SphereGouraud:
            // cameraPos 0.0 0.7 4.0
            resetDepthBuffer();
            // renderSphereRasterised(window, sTriangles);
            // renderSphereWireframe(window, sTriangles);
            gouraudShading(window, sphereCamPos, cameraOrientation, lightPosition, 2.0, sphereTriangles);
            break;
        case RenderingMode::SpherePhong:
            phongShading(window, sphereCamPos, cameraOrientation, lightPosition, 2.0, sphereTriangles);
            resetDepthBuffer();
            break;
    }
}


void handleEvent(SDL_Event event, DrawingWindow &window, glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation, glm::vec3 &lightSource) {

    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_LEFT) {
            // make the camera move left
            std::cout << "Camera moving LEFT" << std::endl;
            window.clearPixels();
            resetDepthBuffer();
            translateCamera(cameraPosition, -0.1, 0, 0);
            std::cout << cameraPosition.x << " " << cameraPosition.y << " " << cameraPosition.z << std::endl;
        }
        else if (event.key.keysym.sym == SDLK_RIGHT) {
            // make the camera move right
            std::cout << "Camera moving RIGHT" << std::endl;
            window.clearPixels();
            resetDepthBuffer();
            translateCamera(cameraPosition, 0.1, 0, 0);
            std::cout << cameraPosition.x << " " << cameraPosition.y << " " << cameraPosition.z << std::endl;
        }
        else if (event.key.keysym.sym == SDLK_UP) {
            // make the camera move up
            std::cout << "Camera moving UP" << std::endl;
            window.clearPixels();
            resetDepthBuffer();
            translateCamera(cameraPosition, 0, 0.1, 0);
            std::cout << cameraPosition.x << " " << cameraPosition.y << " " << cameraPosition.z << std::endl;
        }
        else if (event.key.keysym.sym == SDLK_DOWN) {
            // make the camera move down
            std::cout << "Camera moving DOWN" << std::endl;
            window.clearPixels();
            resetDepthBuffer();
            translateCamera(cameraPosition, 0, -0.1, 0);
            std::cout << cameraPosition.x << " " << cameraPosition.y << " " << cameraPosition.z << std::endl;

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
            rotateCameraByX(cameraPosition);
        }
        else if (event.key.keysym.sym == SDLK_w) {
            window.clearPixels();
            rotateUp(cameraPosition, cameraOrientation, 0.01);
        }
        // rotation matrix y
        else if (event.key.keysym.sym == SDLK_a) {
            window.clearPixels();
            rotateCameraByY(cameraPosition);
        }
        else if (event.key.keysym.sym == SDLK_s) {
            window.clearPixels();
            rotateClock(cameraPosition, cameraOrientation, 0.01);

        }
        else if (event.key.keysym.sym == SDLK_l) {
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
        // model shifting
        else if (event.key.keysym.sym == SDLK_1) {
            currentMode = RenderingMode::Wireframe;
        }
        else if (event.key.keysym.sym == SDLK_2) {
            currentMode = RenderingMode::Rasterised;
        }
        else if (event.key.keysym.sym == SDLK_3) {
            currentMode = RenderingMode::RayTraced;
        }
        else if (event.key.keysym.sym == SDLK_4) {
            currentMode = RenderingMode::Flat;
        }
        else if (event.key.keysym.sym == SDLK_5) {
            currentMode = RenderingMode::SphereGouraud;
        }
        else if (event.key.keysym.sym == SDLK_6) {
            currentMode = RenderingMode::SpherePhong;
        }
        // light position shifting
        else if (event.key.keysym.sym == SDLK_x) {
            window.clearPixels();
            resetDepthBuffer();
            lightSource += glm::vec3(0.1, 0, 0);
            std::cout << lightSource.x << " " << lightSource.y << " " << lightSource.z << std::endl;
        }
        else if (event.key.keysym.sym == SDLK_c) {
            window.clearPixels();
            resetDepthBuffer();
            lightSource += glm::vec3(-0.1, 0, 0);
            std::cout << lightSource.x << " " << lightSource.y << " " << lightSource.z << std::endl;
        }
        else if (event.key.keysym.sym == SDLK_v) {
            window.clearPixels();
            resetDepthBuffer();
            lightSource += glm::vec3(0.0, 0.1, 0);
            std::cout << lightSource.x << " " << lightSource.y << " " << lightSource.z << std::endl;
        }
        else if (event.key.keysym.sym == SDLK_b) {
            window.clearPixels();
            resetDepthBuffer();
            lightSource += glm::vec3(0.0, -0.1, 0);
            std::cout << lightSource.x << " " << lightSource.y << " " << lightSource.z << std::endl;
        }
        else if (event.key.keysym.sym == SDLK_n) {
            window.clearPixels();
            resetDepthBuffer();
            lightSource += glm::vec3(0.0, 0, 0.1);
            std::cout << lightSource.x << " " << lightSource.y << " " << lightSource.z << std::endl;
        }
        else if (event.key.keysym.sym == SDLK_m) {
            window.clearPixels();
            resetDepthBuffer();
            lightSource += glm::vec3(0.0, 0, -0.1);
            std::cout << lightSource.x << " " << lightSource.y << " " << lightSource.z << std::endl;
        }

    } else if (event.type == SDL_MOUSEBUTTONDOWN) {
        window.savePPM("output.ppm");
        window.saveBMP("output.bmp");
    }
}

int main(int argc, char *argv[]) {
    glm::mat3 cameraOrientation = glm::mat3(
            glm::vec3(1, 0, 0),  // right
            glm::vec3(0, 1, 0),  // up
            glm::vec3(0, 0, 1)  // forward
    );

    glm::vec3 cameraPosition = glm::vec3 (0.0, 0.0, 4.0);

    glm::vec3 lightPosition = glm::vec3(0.0, 0.3, 0.3);
    std::vector<ModelTriangle> modelTriangles = readFiles("../src/files/cornell-box.obj", "../src/files/cornell-box.mtl", 0.35);
    // std::vector<std::pair<CanvasTriangle, Colour>> triangles = triangleTransformer(modelTriangles, cameraPosition, cameraOrientation);

    std::vector<ModelTriangle> sphereTriangles = readSphereFile("../src/files/sphere.obj",0.5);
    // std::vector<std::pair<CanvasTriangle, Colour>> sphereCanvasTriangles = triangleTransformer(sphereTriangles, cameraPosition, cameraOrientation);

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
//        resetDepthBuffer();
//        window.clearPixels();
        // We MUST poll for events - otherwise the window will freeze !
        if (window.pollForInputEvents(event)) handleEvent(event, window, cameraPosition, cameraOrientation, lightPosition);
        draw(window, cameraPosition, cameraOrientation, modelTriangles, sphereTriangles, lightPosition);
        // Need to render the frame at the end, or nothing actually gets shown on the screen !
        window.renderFrame();
    }
}