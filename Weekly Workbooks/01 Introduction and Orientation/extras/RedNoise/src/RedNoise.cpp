#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <vector>
#include "glm/vec3.hpp"
#include <CanvasPoint.h>
#include <Colour.h>

#define WIDTH 320
#define HEIGHT 240

// Week2
std::vector<glm::vec3> interpolateThreeElementValues(const glm::vec3& from, const glm::vec3& to, int numberOfValues) {
    if (numberOfValues <= 1) {
        std::cerr << "Error: numberOfValues must be greater than 1." << std::endl;
        return {}; // Return an empty vector in case of error
    }

    std::vector<glm::vec3> result;
    glm::vec3 step = (to - from) / static_cast<float>(numberOfValues - 1);

    for (int i = 0; i < numberOfValues; ++i) {
        result.push_back(from + glm::vec3(i * step.x, i * step.y, i * step.z));
    }

    return result;
}

// return an evenly spaced list of size numberOfValues
std::vector<float> interpolateSingleFloats(float from, float to, int numberOfValues){

    if (numberOfValues <= 1) {
        std::cerr << "Error: numberOfValues must be greater than 1." << std::endl;
        return {}; // Return an empty vector in case of error
    }

    std::vector<float> result;
    float step = (to - from) / (float(numberOfValues) - 1);

    for (int i = 0; i < numberOfValues; ++i) {
        result.push_back(from + i * step);
    }

    return result;
}

void draw(DrawingWindow &window) {
    window.clearPixels();
    // create a vector of floats from 255 to 0 left to right
    std::vector<float> interpolationValues = interpolateSingleFloats(255.f, 0.0f, window.width);

    // create 4 corner values
    glm::vec3 topLeft(255, 0, 0);        // red
    glm::vec3 topRight(0, 0, 255);       // blue
    glm::vec3 bottomRight(0, 255, 0);    // green
    glm::vec3 bottomLeft(255, 255, 0);   // yellow

    // Create vectors
    std::vector<glm::vec3> interpolationValuesLeft = interpolateThreeElementValues(topLeft, bottomLeft, window.height);
    std::vector<glm::vec3> interpolationValuesRight = interpolateThreeElementValues(topRight, bottomRight, window.height);

    for (size_t y = 0; y < window.height; y++) {
        // Create a vector of interpolated RGB colors for the current row
        std::vector<glm::vec3> interpolationValuesRow = interpolateThreeElementValues(interpolationValuesLeft[y], interpolationValuesRight[y], window.width);

        for (size_t x = 0; x < window.width; x++) {
            glm::vec3 color = interpolationValuesRow[x];

            // Convert RGB color to uint32_t
            uint32_t red = static_cast<uint32_t>(color.r);
            uint32_t green = static_cast<uint32_t>(color.g);
            uint32_t blue = static_cast<uint32_t>(color.b);
            uint32_t alpha = 255;
            uint32_t pixelColor = (alpha << 24) + (red << 16) + (green << 8) + blue;

            window.setPixelColour(x, y, pixelColor);
        }
    }
}

// Week3

uint32_t colourConverter(Colour colour) {
    return (255 << 24) + (colour.red << 16) + (colour.green << 8) + colour.blue;
}

// Task 2
void drawLine(DrawingWindow &window, CanvasPoint from, CanvasPoint to, Colour colour) {
    float xDiff = to.x - from.x;
    float yDiff = to.y - from.y;
    float numberOfSteps = std::max(abs(xDiff), abs(yDiff));
    float xStepSize = xDiff / numberOfSteps;
    float yStepSize = yDiff / numberOfSteps;
    uint32_t BLACK = colour.red << 16 | colour.green << 8 | colour.blue;
    for (float i = 0.0; i < numberOfSteps; i++) {
        float x = from.x + (xStepSize * i);
        float y = from.y + (yStepSize * i);
        window.setPixelColour(round(x), round(y), BLACK);
    }
}
// Task 3
void drawStrokedTriangle(DrawingWindow &window, CanvasTriangle triangle, Colour colour) {
    drawLine(window, triangle[0], triangle[1], colour);
    drawLine(window, triangle[1], triangle[2], colour);
    drawLine(window, triangle[2], triangle[0], colour);
}

void drawFilledTriangle(DrawingWindow &window, const CanvasPoint top, const CanvasPoint left, const CanvasPoint right, const CanvasPoint bottom, const Colour &fillColour){

    std::vector<float> rightSide = interpolateSingleFloats(top.x, right.x, int(right.y) - int(top.y));
    std::vector<float> leftSide = interpolateSingleFloats(top.x, left.x, int(left.y) - int(top.y));
    for (int i = top.y; i < int(right.y); i++) {
        CanvasPoint f = CanvasPoint(leftSide.at(i - top.y), i);
        CanvasPoint t = CanvasPoint(rightSide.at(i - top.y), i);
        drawLine(window, f, t, fillColour);
    }
    // color bottom triangle
    rightSide = interpolateSingleFloats(right.x, bottom.x, int(bottom.y) - int(right.y));
    leftSide = interpolateSingleFloats(left.x, bottom.x, int(bottom.y) - int(left.y));
    for (int i = left.y; i < int(bottom.y); i++) {
        CanvasPoint f = CanvasPoint(leftSide.at(i - left.y), i);
        CanvasPoint t = CanvasPoint(rightSide.at(i - left.y), i);
        drawLine(window, f, t, fillColour);
    }
}

void calculateExtraPoint(DrawingWindow &window, const CanvasTriangle &triangle, const Colour &fillColour) {
    // Sort vertices by y-coordinate
    CanvasTriangle sortedTriangle = triangle;
    std::sort(sortedTriangle.vertices.begin(), sortedTriangle.vertices.end(),
              [](const CanvasPoint &a, const CanvasPoint &b) {
                  return a.y < b.y;
              });

    CanvasPoint top = sortedTriangle[0];
    CanvasPoint middle = sortedTriangle[1];
    CanvasPoint bottom = sortedTriangle[2];

    // calculate the extra point on the edge extraPoint.y = middle.y
    float ratio = (bottom.x - top.x)/(bottom.y - top.y);
    float extraPointX= 0;

    if (ratio < 0 ){
        extraPointX = top.x - (middle.y - top.y) * (abs(ratio));
    }else{
        extraPointX = top.x + (middle.y - top.y) * (abs(ratio));
    }

    CanvasPoint extraPoint(extraPointX, middle.y);
    drawFilledTriangle(window, top, middle, extraPoint, bottom, fillColour);

}


void handleEvent(SDL_Event event, DrawingWindow &window) {
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_LEFT) std::cout << "LEFT" << std::endl;
        else if (event.key.keysym.sym == SDLK_RIGHT) std::cout << "RIGHT" << std::endl;
        else if (event.key.keysym.sym == SDLK_UP) std::cout << "UP" << std::endl;
        else if (event.key.keysym.sym == SDLK_DOWN) std::cout << "DOWN" << std::endl;
        else if (event.key.keysym.sym == SDLK_u) {

            // Generate random vertices for the triangle
            CanvasPoint v0(rand() % window.width, rand() % window.height);
            CanvasPoint v1(rand() % window.width, rand() % window.height);
            CanvasPoint v2(rand() % window.width, rand() % window.height);
            CanvasTriangle triangle(v0, v1, v2);

            Colour color(rand() % 256, rand() % 256, rand() % 256);

            drawStrokedTriangle(window, triangle, color);

            window.renderFrame();
        }
        else if (event.key.keysym.sym == SDLK_f) {
            // Generate random vertices for the filled triangle
            CanvasPoint v0(rand() % window.width, rand() % window.height);
            CanvasPoint v1(rand() % window.width, rand() % window.height);
            CanvasPoint v2(rand() % window.width, rand() % window.height);
            CanvasTriangle triangle(v0, v1, v2);

            // Draw a stroked triangle over the filled triangle
            Colour strokeColor(255, 255, 255); // white frame
            drawStrokedTriangle(window, triangle, strokeColor);

            // Generate a random color for filling
            Colour fillColor(rand() % 256, rand() % 256, rand() % 256);
            calculateExtraPoint(window, triangle, fillColor);

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

    // test for interpolateSingleFloats
    std::vector<float> result;
    result = interpolateSingleFloats(2.2, 8.5, 7);
    for(size_t i=0; i<result.size(); i++) std::cout << result[i] << " ";
    std::cout << std::endl;

    //test for interpolateThreeElementValues
    glm::vec3 from(1.0f, 4.0f, 9.2f);
    glm::vec3 to(4.0f, 1.0f, 9.8f);
    int numberOfValues = 4; // Adjust the number of values as needed

    std::vector<glm::vec3> interpolatedValues = interpolateThreeElementValues(from, to, numberOfValues);

    for (const glm::vec3& value : interpolatedValues) {
        std::cout << "(" << value.x << ", " << value.y << ", " << value.z << ") ";
    }
    std::cout << std::endl;

    while (true) {
        // We MUST poll for events - otherwise the window will freeze !
        if (window.pollForInputEvents(event)) handleEvent(event, window);
        draw(window);
        // Need to render the frame at the end, or nothing actually gets shown on the screen !
        window.renderFrame();
    }
}