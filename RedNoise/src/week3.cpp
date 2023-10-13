#include "week2.h"
#include "week3.h"

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

void drawFilledTriangle(DrawingWindow &window, const CanvasPoint top, const CanvasPoint left, const CanvasPoint right, const Colour &fillColour) {
    // Calculate the slope of the left and right edges
    float slope_left = (left.x - top.x) / (left.y - top.y);
    float slope_right = (right.x - top.x) / (right.y - top.y);

    // Fill the top half of the triangle
    for (int y = top.y; y <= left.y; y++) {
        int x1 = top.x + (y - top.y) * slope_left;
        int x2 = top.x + (y - top.y) * slope_right;
        drawLine(window, CanvasPoint(x1, y), CanvasPoint(x2, y), fillColour);
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
    drawFilledTriangle(window, top, middle, extraPoint, fillColour);
    drawFilledTriangle(window, bottom, middle,extraPoint, fillColour);
}
