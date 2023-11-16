#ifndef SPHERE_H
#define SPHERE_H

#include "ray.h"



std::vector<ModelTriangle> readSphereFile (const std::string &filename, float scale);
void renderSphereWireframe(DrawingWindow &window, std::vector<std::pair<CanvasTriangle, Colour>> &sTriangles);
void renderSphereRasterised(DrawingWindow &window, std::vector<std::pair<CanvasTriangle, Colour>> &sTriangles);
void gouraudShading (DrawingWindow &window, glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation,
                     const glm::vec3 &lightSource, float focalLength, std::vector<ModelTriangle> &sphereTriangles);
void phongShading (DrawingWindow &window, glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation,
                   const glm::vec3 &lightSource, float focalLength, std::vector<ModelTriangle> &sphereTriangles);
#endif //SPHERE_H
