#ifndef WEEK3_H
#define WEEK3_H

#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <vector>
#include "glm/vec3.hpp"
#include <CanvasPoint.h>
#include <Colour.h>

// Function to convert a Colour object to a uint32_t value
uint32_t colourConverter(Colour colour);

// Task 2: Function to draw a line between two CanvasPoints with a specified Colour
void drawLine(DrawingWindow &window, CanvasPoint from, CanvasPoint to, Colour colour);

// Task 3: Function to draw a stroked triangle using three CanvasPoints and a Colour
void drawStrokedTriangle(DrawingWindow &window, CanvasTriangle triangle, Colour colour);

// Function to draw a filled triangle using four CanvasPoints and a fill Colour
void drawFilledTriangle(DrawingWindow &window, const CanvasPoint top, const CanvasPoint left, const CanvasPoint right, const CanvasPoint bottom, const Colour &fillColour);

// Function to calculate and draw the extra point to fill the triangle
void calculateExtraPoint(DrawingWindow &window, const CanvasTriangle &triangle, const Colour &fillColour);

#endif // WEEK3_H
