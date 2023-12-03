#include "interpolation.h"
#include "triangle.h"
#include "wireframes.h"
#include "ray.h"
#include "sphere.h"
#include "logo.h"

#define WIDTH 640
#define HEIGHT 480

bool orbitClockwiseActivated = false;
bool orbitUpActivated = false;
bool orbitSelfActivated = false;

enum class RenderingMode { Wireframe, Rasterised, RayTraced, Flat, SphereGouraud, SpherePhong, Logo };
RenderingMode currentMode = RenderingMode::Rasterised;

shading shadingType = Phong;
shadow shadowType = None;

void draw(DrawingWindow &window, glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation, glm::vec3 &lightPosition,
          std::vector<ModelTriangle> &modelTriangles, std::vector<ModelTriangle> &sphereTriangles, std::vector<ModelTriangle> &logoTriangle, std::vector<ModelTriangle> &completeModel) {
    // Cornell Box's modelTriangles
    std::vector<std::pair<CanvasTriangle, Colour>> triangles = triangleTransformer(modelTriangles, cameraPosition, cameraOrientation);
    // complete modelTriangles
    std::vector<std::pair<CanvasTriangle, Colour>> cTriangles = triangleTransformer(completeModel, cameraPosition, cameraOrientation);
    // test processTriangles function
    std::vector<triangleData> triangleDatas = processTriangles(modelTriangles, cameraPosition, cameraOrientation);

    glm::vec3 sphereCamPos = glm::vec3(0.0, 0.0, 4.0);
    glm::vec3 sphereLightPos = glm::vec3(0.5, 0.8, 1.6);
    // std::vector<std::pair<CanvasTriangle, Colour>> sTriangles = triangleTransformer(sphereTriangles, sphereCamPos, cameraOrientation);
    if(orbitClockwiseActivated){
        orbitClockwise(cameraPosition, cameraOrientation, 0.01);
    }
    if (orbitUpActivated){
        orbitUp(cameraPosition, cameraOrientation, 0.01);
    }
    if (orbitSelfActivated){
        orbitSelf(cameraPosition, cameraOrientation, 0.01);
    }

    window.clearPixels();

    switch (currentMode) {
        case RenderingMode::Wireframe:
            resetDepthBuffer();
            renderWireframe(window, cTriangles);
            break;
        case RenderingMode::Rasterised:
            resetDepthBuffer();
            // renderRasterised(window,  triangles);
            testProcess(window, triangleDatas);
            break;
        case RenderingMode::RayTraced:
            resetDepthBuffer();
            drawRayTracedScene(window, cameraPosition, cameraOrientation, 2.0, modelTriangles, lightPosition);
            break;
        case RenderingMode::Flat:
            resetDepthBuffer();
            rayTrace(window, cameraPosition, cameraOrientation, lightPosition, completeModel, shadingType, shadowType);
            break;
        case RenderingMode::SphereGouraud:
            // cameraPos 0.0 0.7 4.0

            resetDepthBuffer();
            // renderSphereRasterised(window, sTriangles);
            // renderSphereWireframe(window, sTriangles);
            gouraudShading(window, cameraPosition, cameraOrientation, lightPosition, 2.0, sphereTriangles);
            break;
        case RenderingMode::SpherePhong:
            resetDepthBuffer();
            phongShading(window, cameraPosition, cameraOrientation, lightPosition, 2.0, completeModel);
            break;

        case RenderingMode::Logo: // logoCamPos 0.3 0.3 4.0
            resetDepthBuffer();
            drawRayTracedSceneSoft(window, cameraPosition, cameraOrientation, 2.0, completeModel, lightPosition);
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
        else if (event.key.keysym.sym == SDLK_l) {
            // make the camera move forward
            std::cout << "Camera moving FORWARD" << std::endl;
            window.clearPixels();
            resetDepthBuffer();
            translateCamera(cameraPosition, 0, 0, -0.1);
            std::cout << cameraPosition.x << " " << cameraPosition.y << " " << cameraPosition.z << std::endl;

        }
        else if (event.key.keysym.sym == SDLK_k) {
            // make the camera move backward
            std::cout << "Camera moving BACKWARD" << std::endl;
            window.clearPixels();
            resetDepthBuffer();
            translateCamera(cameraPosition, 0, 0, 0.1);
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
        else if (event.key.keysym.sym == SDLK_i) {
            window.clearPixels();
            orbitSelfActivated = !orbitSelfActivated;
        }
        else if (event.key.keysym.sym == SDLK_o) {
            window.clearPixels();
            orbitClockwiseActivated = !orbitClockwiseActivated;
        }
        else if (event.key.keysym.sym == SDLK_p) {
            window.clearPixels();
            orbitUpActivated = !orbitUpActivated;
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
        else if (event.key.keysym.sym == SDLK_7) {
            currentMode = RenderingMode::Logo;
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
std::vector<ModelTriangle> mergeModelTriangles(const std::vector<ModelTriangle>& model1, const std::vector<ModelTriangle>& model2) {
    std::vector<ModelTriangle> mergedModel = model1;
    mergedModel.insert(mergedModel.end(), model2.begin(), model2.end());
    return mergedModel;
}



int main(int argc, char *argv[]) {
    glm::mat3 cameraOrientation = glm::mat3(
            glm::vec3(1, 0, 0),  // right
            glm::vec3(0, 1, 0),  // up
            glm::vec3(0, 0, 1)  // forward
    );

    glm::vec3 cameraPosition = glm::vec3 (0.0, 0.0, 4.0);

    glm::vec3 lightPosition = glm::vec3(0.0, 0.3, 0.3);
    std::vector<ModelTriangle> modelTriangles = readFiles("../src/files/textured-cornell-box.obj", "../src/files/textured-cornell-box.mtl", 0.35);
    // std::vector<std::pair<CanvasTriangle, Colour>> triangles = triangleTransformer(modelTriangles, cameraPosition, cameraOrientation);

    std::vector<ModelTriangle> sphereTriangles = readSphereFile("../src/files/sphere_updated.obj",0.35);
    glm::vec3 sphereCenter = calculateSphereCenter(sphereTriangles);
    float sphereRadius = calculateSphereRadius(sphereTriangles, sphereCenter);
    std::cout << "sphere center: " << sphereCenter.x << " " << sphereCenter.y << " " << sphereCenter.z << std::endl;
    std::cout << "sphere radius: " << sphereRadius << std::endl;
    // std::vector<std::pair<CanvasTriangle, Colour>> sphereCanvasTriangles = triangleTransformer(sphereTriangles, cameraPosition, cameraOrientation);
    std::vector<ModelTriangle> logoTriangles = readLogoFile("../src/files/logo_updated.obj",0.0015);
    std::vector<ModelTriangle> mergedModel = mergeModelTriangles(modelTriangles, logoTriangles);
    std::vector<ModelTriangle> completeModel = mergeModelTriangles(mergedModel, sphereTriangles);

    DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
    SDL_Event event;

    while (true) {
//        resetDepthBuffer();
//        window.clearPixels();
        // We MUST poll for events - otherwise the window will freeze !
        if (window.pollForInputEvents(event)) handleEvent(event, window, cameraPosition, cameraOrientation, lightPosition);
        draw(window, cameraPosition, cameraOrientation, lightPosition, modelTriangles, sphereTriangles, logoTriangles, completeModel );
        // Need to render the frame at the end, or nothing actually gets shown on the screen !
        window.renderFrame();
    }
}