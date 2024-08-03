#ifndef RAY_H
#define RAY_H

#define WIDTH 640
#define HEIGHT 480

#include "wireframes.h"
#include "RayTriangleIntersection.h"
#include "DrawingWindow.h"
#include "triangle.h"
#include "sphere.h"
#include "logo.h"

// Week 6
RayTriangleIntersection getClosestValidIntersection(const glm::vec3 &cameraPosition, const glm::vec3 &rayDirection,
                                                            const std::vector<ModelTriangle> &modelTriangles);
glm::vec3 getDirection(glm::vec3 cameraPosition, glm::mat3 cameraOrientation, float x, float y, float focalLength);

void processPixel(DrawingWindow &window, const glm::vec3 &cameraPosition, const glm::mat3 &cameraOrientation,
                  float x, float y, float focalLength, const std::vector<ModelTriangle> &modelTriangles, const glm::vec3 &lightPosition, float intensity);
void processPixelSoft(DrawingWindow &window, const glm::vec3 &cameraPosition, const glm::mat3 &cameraOrientation,
                  float x, float y, float focalLength, const std::vector<ModelTriangle> &modelTriangles, const std::vector<glm::vec3> &lightPositions, float intensity);

void drawRayTracedScene (DrawingWindow &window, glm::vec3 &cameraPosition, glm::mat3 cameraOrientation,
                         float focalLength, const std::vector<ModelTriangle> &modelTriangles, glm::vec3 lightSource, float intensity);
void drawRayTracedSceneSoft (DrawingWindow &window, glm::vec3 &cameraPosition, glm::mat3 cameraOrientation,
                             float focalLength, const std::vector<ModelTriangle> &modelTriangles, glm::vec3 lightSource, float intensity);
// Week 7
glm::vec3 getTriangleNormal (const ModelTriangle &modelTriangle);

float processLighting(const glm::vec3 &lightDistance, const glm::vec3 &normal, glm::vec3 view, float intensity);

std::vector <glm::vec3> multipleLightSources (glm::vec3 LightPosition);
#endif //RAY_H
