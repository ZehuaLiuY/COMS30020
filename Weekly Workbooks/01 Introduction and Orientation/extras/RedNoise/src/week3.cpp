#include "week3.h"


// Week3
// convert a Colour object to a uint32_t value
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
    uint32_t uIntColour = colourConverter(colour);
    for (float i = 0.0; i < numberOfSteps; i++) {
        float x = from.x + (xStepSize * i);
        float y = from.y + (yStepSize * i);
        window.setPixelColour(round(x), round(y), uIntColour);
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

void drawFilledTriangle(DrawingWindow &window, const CanvasTriangle &triangle, const Colour &fillColour) {
    std::array<CanvasPoint, 4> sortedPoints = calculateExtraPoint(triangle);

    CanvasPoint top = sortedPoints[0];
    CanvasPoint left = sortedPoints[1];
    CanvasPoint right = sortedPoints[2];
    CanvasPoint bottom = sortedPoints[3];

    // Calculate the slope of the left and right edges
    float slope_left = (left.x - top.x) / (left.y - top.y);
    float slope_right = (right.x - top.x) / (right.y - top.y);

    // Fill the top half of the triangle
    for (int y = top.y; y <= left.y; y++) {
        float x1 = top.x + (y - top.y) * slope_left;
        float x2 = top.x + (y - top.y) * slope_right;
        drawLine(window, CanvasPoint(round(x1), y), CanvasPoint(round(x2), y), fillColour);
    }

    // fill the bottom half of the triangle
    float slope_left_bottom = (bottom.x - left.x) / (bottom.y - left.y);
    float slope_right_bottom = (bottom.x - right.x) / (bottom.y - right.y);
    for (int y = left.y; y <= bottom.y; y++) {
        float x1 = left.x + (y - left.y) * slope_left_bottom;
        float x2 = right.x + (y - right.y) * slope_right_bottom;
        drawLine(window, CanvasPoint(round(x1), y), CanvasPoint(round(x2), y), fillColour);
    }
}

// Task 5

void drawTexturedLine(DrawingWindow &window, CanvasPoint start, CanvasPoint end, TextureMap &texture) {
    float xDiff = end.x - start.x;
    float yDiff = end.y - start.y;
    float numberOfSteps = std::max(abs(xDiff), abs(yDiff));
    float xStepSize = xDiff / numberOfSteps;
    float yStepSize = yDiff / numberOfSteps;

    // Manually subtract TexturePoint objects
    float texDiffX = end.texturePoint.x - start.texturePoint.x;
    float texDiffY = end.texturePoint.y - start.texturePoint.y;

    // Divide TexturePoint components by scalar
    float texStepSizeX = texDiffX / numberOfSteps;
    float texStepSizeY = texDiffY / numberOfSteps;

    for (float i = 0.0; i < numberOfSteps; i++) {
        float x = start.x + (xStepSize * i);
        float y = start.y + (yStepSize * i);

        // Manually add scaled TexturePoint components
        float texX = start.texturePoint.x + (texStepSizeX * i);
        float texY = start.texturePoint.y + (texStepSizeY * i);

        int texIndex = round(texY) * texture.width + round(texX);
        uint32_t texPixel = texture.pixels[texIndex];

        window.setPixelColour(round(x), round(y), texPixel);
    }
}


void drawTexturedTriangle(DrawingWindow &window, const CanvasTriangle &triangle, TextureMap &texture) {
    std::array<CanvasPoint, 4> sortedPoints = calculateExtraPoint(triangle);

    CanvasPoint top = sortedPoints[0];
    CanvasPoint left = sortedPoints[1];
    CanvasPoint right = sortedPoints[2];
    CanvasPoint bottom = sortedPoints[3];

    // Calculate the slope of the left and right edges for x coordinates
    float slope_left = (left.x - top.x) / (left.y - top.y);
    float slope_right = (right.x - top.x) / (right.y - top.y);

    // Calculate the slope of the left and right edges for texture coordinates
    float texSlope_left_x = (left.texturePoint.x - top.texturePoint.x) / (left.y - top.y);
    float texSlope_right_x = (right.texturePoint.x - top.texturePoint.x) / (right.y - top.y);
    float texSlope_left_y = (left.texturePoint.y - top.texturePoint.y) / (left.y - top.y);
    float texSlope_right_y = (right.texturePoint.y - top.texturePoint.y) / (right.y - top.y);

    // Fill the top half of the triangle
    for (int y = top.y; y <= left.y; y++) {
        float x1 = top.x + (y - top.y) * slope_left;
        float x2 = top.x + (y - top.y) * slope_right;

        CanvasPoint point1(round(x1), y);
        CanvasPoint point2(round(x2), y);

        point1.texturePoint = TexturePoint(top.texturePoint.x + (y - top.y) * texSlope_left_x,
                                           top.texturePoint.y + (y - top.y) * texSlope_left_y);
        point2.texturePoint = TexturePoint(top.texturePoint.x + (y - top.y) * texSlope_right_x,
                                           top.texturePoint.y + (y - top.y) * texSlope_right_y);

        drawTexturedLine(window, point1, point2, texture);
    }

    // Calculate the slope for the bottom half
    float slope_left_bottom = (bottom.x - left.x) / (bottom.y - left.y);
    float slope_right_bottom = (bottom.x - right.x) / (bottom.y - right.y);

    float texSlope_left_bottom_x = (bottom.texturePoint.x - left.texturePoint.x) / (bottom.y - left.y);
    float texSlope_right_bottom_x = (bottom.texturePoint.x - right.texturePoint.x) / (bottom.y - right.y);
    float texSlope_left_bottom_y = (bottom.texturePoint.y - left.texturePoint.y) / (bottom.y - left.y);
    float texSlope_right_bottom_y = (bottom.texturePoint.y - right.texturePoint.y) / (bottom.y - right.y);

    // Fill the bottom half of the triangle
    for (int y = left.y; y <= bottom.y; y++) {
        float x1 = left.x + (y - left.y) * slope_left_bottom;
        float x2 = right.x + (y - right.y) * slope_right_bottom;

        CanvasPoint point1(round(x1), y);
        CanvasPoint point2(round(x2), y);

        point1.texturePoint = TexturePoint(left.texturePoint.x + (y - left.y) * texSlope_left_bottom_x,
                                           left.texturePoint.y + (y - left.y) * texSlope_left_bottom_y);
        point2.texturePoint = TexturePoint(right.texturePoint.x + (y - right.y) * texSlope_right_bottom_x,
                                           right.texturePoint.y + (y - right.y) * texSlope_right_bottom_y);

        drawTexturedLine(window, point1, point2, texture);
    }
}

// Visual Verification
void testTexturedTriangle(DrawingWindow &window) {
    TextureMap tm = TextureMap("../src/files/texture.ppm");
    CanvasPoint top = CanvasPoint(160, 10);
    top.texturePoint = TexturePoint(195, 5);
    CanvasPoint mid = CanvasPoint(10, 150);
    mid.texturePoint = TexturePoint(65, 330);
    CanvasPoint bottom = CanvasPoint(300, 230);
    bottom.texturePoint = TexturePoint(395, 380);
    CanvasTriangle triangle = CanvasTriangle(top, mid, bottom);
    drawTexturedTriangle(window, triangle, tm);
    drawStrokedTriangle(window, triangle, Colour(255, 255, 255));;
}