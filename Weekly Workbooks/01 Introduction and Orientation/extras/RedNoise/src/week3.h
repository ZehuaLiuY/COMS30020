#ifndef WEEK3_H
#define WEEK3_H

#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <vector>
#include "glm/glm.hpp"
#include <CanvasPoint.h>
#include <Colour.h>
#include <TextureMap.h>

// Function to convert a Colour object to a uint32_t value
uint32_t colourConverter(Colour colour);

// Task 2: Function to draw a line between two CanvasPoints with a specified Colour
void drawLine(DrawingWindow &window, CanvasPoint from, CanvasPoint to, Colour colour);

// Task 3: Function to draw a stroked triangle using three CanvasPoints and a Colour

CanvasTriangle generateRandomTriangle(DrawingWindow &window);

void drawStrokedTriangle(DrawingWindow &window, CanvasTriangle triangle, Colour colour);

// Task 4: Function to calculate and draw the extra point to fill the triangle
std::array<CanvasPoint, 4> calculateExtraPoint(const CanvasTriangle &triangle);

// Function to draw a filled triangle using four CanvasPoints and a fill Colour
void drawFilledTriangle(DrawingWindow &window, const CanvasTriangle &triangle, const Colour &fillColour);

// Task 5: Function to draw a filled triangle using three CanvasPoints and a fill Colour
void drawTexturedLine(DrawingWindow &window, CanvasPoint start, CanvasPoint end, TextureMap &texture);
void drawTexturedTriangle(DrawingWindow &window, const CanvasTriangle &triangle, TextureMap &texture);
void testTexturedTriangle(DrawingWindow &window);
#endif // WEEK3_H
