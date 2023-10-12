#ifndef WEEK2_H
#define WEEK2_H

#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <vector>
#include "glm/vec3.hpp"

// Function to interpolate three-element values
std::vector<glm::vec3> interpolateThreeElementValues(const glm::vec3& from, const glm::vec3& to, int numberOfValues);

// Function to interpolate single floating-point values
std::vector<float> interpolateSingleFloats(float from, float to, int numberOfValues);

// Function to draw on a DrawingWindow
void drawRGBColour(DrawingWindow &window);

#endif // WEEK2_H

