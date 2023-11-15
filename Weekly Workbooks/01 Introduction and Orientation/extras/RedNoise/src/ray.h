#ifndef RAY_H
#define RAY_H

#define WIDTH 640
#define HEIGHT 480

#include "wireframes.h"
#include "RayTriangleIntersection.h"
#include "DrawingWindow.h"
#include "triangle.h"

RayTriangleIntersection getClosestValidIntersection(const glm::vec3 &cameraPosition, const glm::vec3 &rayDirection,
                                                            const std::vector<ModelTriangle> &modelTriangles);
void drawRayTracedScene (DrawingWindow &window, glm::vec3 &cameraPosition, glm::mat3 cameraOrientation,
                         float focalLength, const std::vector<ModelTriangle> &modelTriangles, glm::vec3 lightSource);

// Week 7
glm::vec3 getTriangleNormal (const ModelTriangle &modelTriangle);

#endif //RAY_H
