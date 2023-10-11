#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <vector>
#include "glm/vec3.hpp"
#include <CanvasPoint.h>
#include <Colour.h>

#define WIDTH 320
#define HEIGHT 240

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

// Task 4 Draw a filled triangle with stroked triangle
void swap(CanvasPoint &a, CanvasPoint &b) {
    CanvasPoint temp = a;
    a = b;
    b = temp;
}

// Task 5: Helper function to sort CanvasPoints by their y-coordinates
void sortPointsByY(CanvasPoint &a, CanvasPoint &b, CanvasPoint &c) {
    if (a.y > b.y) swap(a, b);
    if (a.y > c.y) swap(a, c);
    if (b.y > c.y) swap(b, c);
}

void drawFilledTriangle(DrawingWindow &window, CanvasTriangle triangle, Colour fillColour) {
    sortPointsByY(triangle[0], triangle[1], triangle[2]);

    bool middleIsLeft = ((triangle[1].x - triangle[0].x) * (triangle[2].y - triangle[0].y) -
                         (triangle[2].x - triangle[0].x) * (triangle[1].y - triangle[0].y)) > 0;

    // Filling top part of the triangle (triangle[0] to triangle[1])
    for (int y = triangle[0].y; y <= triangle[1].y; y++) {
        float alpha = static_cast<float>(y - triangle[0].y) / (triangle[1].y - triangle[0].y);
        float beta = static_cast<float>(y - triangle[0].y) / (triangle[2].y - triangle[0].y);
        CanvasPoint left, right;

        if (middleIsLeft) {
            left.x = triangle[0].x + (triangle[1].x - triangle[0].x) * alpha;
            left.y = y;
            right.x = triangle[0].x + (triangle[2].x - triangle[0].x) * beta;
            right.y = y;
        } else {
            left.x = triangle[0].x + (triangle[2].x - triangle[0].x) * beta;
            left.y = y;
            right.x = triangle[0].x + (triangle[1].x - triangle[0].x) * alpha;
            right.y = y;
        }

        if (left.x > right.x) swap(left, right);

        for (int x = static_cast<int>(left.x); x <= static_cast<int>(right.x); x++) {
            window.setPixelColour(x, y, fillColour.red << 16 | fillColour.green << 8 | fillColour.blue);
        }
    }

    // Filling the bottom part of the triangle (triangle[1] to triangle[2])
    for (int y = triangle[1].y + 1; y <= triangle[2].y; y++) {
        float alpha = static_cast<float>(y - triangle[0].y) / (triangle[2].y - triangle[0].y);
        float beta = static_cast<float>(y - triangle[1].y) / (triangle[2].y - triangle[1].y);
        CanvasPoint left, right;

        if (middleIsLeft) {
            left.x = triangle[0].x + (triangle[2].x - triangle[0].x) * beta;
            right.x = triangle[1].x + (triangle[2].x - triangle[1].x) * alpha;
        } else {
            left.x = triangle[1].x + (triangle[2].x - triangle[1].x) * beta;
            right.x = triangle[0].x + (triangle[2].x - triangle[0].x) * alpha;
        }

        if (left.x > right.x) swap(left, right);

        for (int x = static_cast<int>(left.x); x <= static_cast<int>(right.x); x++) {
            window.setPixelColour(x, y, fillColour.red << 16 | fillColour.green << 8 | fillColour.blue);
        }
    }



    drawStrokedTriangle(window, triangle, Colour(255, 255, 255));
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
            // Draw the filled triangle
            drawFilledTriangle(window, triangle, fillColor);


            window.renderFrame();
        }
    } else if (event.type == SDL_MOUSEBUTTONDOWN) {
        window.savePPM("output.ppm");
        window.saveBMP("output.bmp");
    }
}

int main(int argc, char *argv[]) {
    srand(static_cast<unsigned>(time(nullptr))); // Initialize random seed

    DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
    SDL_Event event;

    while (true) {
        // We MUST poll for events - otherwise the window will freeze !
        if (window.pollForInputEvents(event)) handleEvent(event, window);
        // Need to render the frame at the end, or nothing actually gets shown on the screen !
        window.renderFrame();
    }
}