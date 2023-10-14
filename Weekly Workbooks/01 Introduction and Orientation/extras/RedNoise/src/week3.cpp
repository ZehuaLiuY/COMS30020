#include "week3.h"


// Week3
//// convert a Colour object to a uint32_t value
//uint32_t colourConverter(Colour colour) {
//    return (255 << 24) + (colour.red << 16) + (colour.green << 8) + colour.blue;
//}

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

CanvasTriangle generateRandomTriangle(DrawingWindow &window) {
    CanvasPoint v0(rand() % window.width, rand() % window.height);
    CanvasPoint v1(rand() % window.width, rand() % window.height);
    CanvasPoint v2(rand() % window.width, rand() % window.height);
    return CanvasTriangle(v0, v1, v2);
}

void drawStrokedTriangle(DrawingWindow &window, CanvasTriangle triangle, Colour colour) {
    drawLine(window, triangle[0], triangle[1], colour);
    drawLine(window, triangle[1], triangle[2], colour);
    drawLine(window, triangle[2], triangle[0], colour);
}

// Task 4

std::array<CanvasPoint, 4> calculateExtraPoint(const CanvasTriangle &triangle) {
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
    float extraPointX;

    if (ratio < 0 ){
        extraPointX = top.x - (middle.y - top.y) * (abs(ratio));
    }else{
        extraPointX = top.x + (middle.y - top.y) * (abs(ratio));
    }

    CanvasPoint extraPoint(extraPointX, middle.y);
    std::array<CanvasPoint, 4> sortedPoints = {top, middle, extraPoint, bottom};
    return sortedPoints;

}
