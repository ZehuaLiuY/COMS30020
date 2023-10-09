#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <vector>
#include "glm/vec3.hpp"
#include <CanvasPoint.h>
#include <Colour.h>

#define WIDTH 320
#define HEIGHT 240

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

void drawStrokedTriangle(DrawingWindow &window, CanvasTriangle triangle, Colour colour) {
    drawLine(window, triangle[0], triangle[1], colour);
    drawLine(window, triangle[1], triangle[2], colour);
    drawLine(window, triangle[2], triangle[0], colour);
}

void drawFilledTriangle(DrawingWindow &window, const CanvasTriangle &triangle, const Colour &fillColour) {
    // Sort vertices by y-coordinate
    CanvasTriangle sortedTriangle = triangle;
    std::sort(sortedTriangle.vertices.begin(), sortedTriangle.vertices.end(),
              [](const CanvasPoint &a, const CanvasPoint &b) {
                  return a.y < b.y;
              });

    // Get the top, middle and bottom vertices
    CanvasPoint top = sortedTriangle[0];
    CanvasPoint middle = sortedTriangle[1];
    CanvasPoint bottom = sortedTriangle[2];

    // Calculate slopes for the two edges
    float slope1 = (middle.x - top.x) / (middle.y - top.y);
    float slope2 = (bottom.x - top.x) / (bottom.y - top.y);
    float x1 = top.x;
    float x2 = top.x;

    // Loop through scan lines
    for (int y = top.y; y <= middle.y; y++) {
        for (int x = std::round(x1); x <= std::round(x2); x++) {
            // Create a uint32_t color value
            uint32_t pixelColor = fillColour.red << 16 | fillColour.green << 8 | fillColour.blue;
            window.setPixelColour(x, y, pixelColor);
        }

        x1 += slope1;
        x2 += slope2;
    }

    // Reset x values and slopes for the second part of the triangle
    x1 = middle.x;
    slope1 = (bottom.x - middle.x) / (bottom.y - middle.y);

    // Loop through scan lines for the second part of the triangle
    for (int y = middle.y + 1; y <= bottom.y; y++) {
        for (int x = std::round(x1); x <= std::round(x2); x++) {
            // Create a uint32_t color value
            uint32_t pixelColor = fillColour.red << 16 | fillColour.green << 8 | fillColour.blue;
            window.setPixelColour(x, y, pixelColor);
        }

        x1 += slope1;
        x2 += slope2;
    }
}


void draw(DrawingWindow &window) {

//    // Draw lines with different colors
//    Colour WHITE(255, 255, 255);
//    // A line from the top-left corner to the center
//    drawLine(window, CanvasPoint(0, 0), CanvasPoint(WIDTH / 2, HEIGHT / 2), WHITE);
//    // A line from the top-right corner to the center
//    drawLine(window, CanvasPoint(WIDTH, 0), CanvasPoint(WIDTH / 2, HEIGHT / 2), WHITE);
//    // A vertical line down the middle of the screen
//    drawLine(window, CanvasPoint(WIDTH / 2, 0), CanvasPoint(WIDTH / 2, HEIGHT), WHITE);
//
//    // A horizontal line centered both horizontally and vertically
//    int centerX = WIDTH / 2;
//    int centerY = HEIGHT / 2;
//    int lineLength = WIDTH / 3;
//    drawLine(window, CanvasPoint(centerX - lineLength / 2, centerY),
//             CanvasPoint(centerX + lineLength / 2, centerY), WHITE);
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

            // Generate a random color for filling
            Colour fillColor(rand() % 256, rand() % 256, rand() % 256);

            // Draw the filled triangle
            drawFilledTriangle(window, triangle, fillColor);
            // Draw a stroked triangle over the filled triangle
            Colour strokeColor(255, 255, 255); // white frame
            drawStrokedTriangle(window, triangle, strokeColor);

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
		draw(window);
		// Need to render the frame at the end, or nothing actually gets shown on the screen !
		window.renderFrame();
	}
}
