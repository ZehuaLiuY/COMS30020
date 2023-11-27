#ifndef RAY_H
#define RAY_H

#define WIDTH 640
#define HEIGHT 480

#include "wireframes.h"
#include "RayTriangleIntersection.h"
#include "DrawingWindow.h"
#include "triangle.h"

// Week 6
RayTriangleIntersection getClosestValidIntersection(const glm::vec3 &cameraPosition, const glm::vec3 &rayDirection,
                                                            const std::vector<ModelTriangle> &modelTriangles);
glm::vec3 getDirection(glm::vec3 cameraPosition, glm::mat3 cameraOrientation, float x, float y, float focalLength);

void processPixel(DrawingWindow &window, const glm::vec3 &cameraPosition, const glm::mat3 &cameraOrientation,
                  float x, float y, float focalLength, const std::vector<ModelTriangle> &modelTriangles, const glm::vec3 &lightPosition);

void drawRayTracedScene (DrawingWindow &window, glm::vec3 &cameraPosition, glm::mat3 cameraOrientation,
                         float focalLength, const std::vector<ModelTriangle> &modelTriangles, glm::vec3 lightSource);

// Week 7
glm::vec3 getTriangleNormal (const ModelTriangle &modelTriangle);
float proximityLighting (float distance);

float getIncidenceAngle (glm::vec3 lightDistance, glm::vec3 normal);

float specularLighting (glm::vec3 lightDistance, glm::vec3 normal, glm::vec3 rayDirection);

float processLighting(const glm::vec3 &lightDistance, const glm::vec3 &normal, glm::vec3 view);
#endif //RAY_H
