#ifndef RAY_H
#define RAY_H

#define WIDTH 320
#define HEIGHT 240

#include "wireframes.h"
#include "RayTriangleIntersection.h"
#include "DrawingWindow.h"
#include "triangle.h"

RayTriangleIntersection getClosestValidIntersection(const glm::vec3 &cameraPosition, const glm::vec3 &rayDirection,
                                                            const std::vector<ModelTriangle> &modelTriangles);
void drawRayTracedScene(DrawingWindow &window, glm::vec3 &cameraPosition,
                        float focalLength, const std::vector<ModelTriangle> &modelTriangles, glm::vec3 lightSource);

#endif //RAY_H
