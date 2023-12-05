#ifndef SPHERE_H
#define SPHERE_H

#include "ray.h"
#include <iostream>


std::vector<ModelTriangle> readSphereFile (const std::string &filename, float scale);

glm::vec3 calculateSphereCenter (const std::vector<ModelTriangle> &sphereTriangles);
float calculateSphereRadius (const std::vector<ModelTriangle> &sphereTriangles, const glm::vec3 &center);

void renderSphereWireframe(DrawingWindow &window, std::vector<std::pair<CanvasTriangle, Colour>> &sTriangles);
void renderSphereRasterised(DrawingWindow &window, std::vector<std::pair<CanvasTriangle, Colour>> &sTriangles);

glm::vec3 getNormalWeight (float x, float y, const ModelTriangle &triangle);
glm::vec3 calculateVertexNormal (glm::vec3 vertex, const std::vector<ModelTriangle> &sphereTriangles);

void flatShading (DrawingWindow &window, glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation, float &intensity,
                  const glm::vec3 &lightPosition, float focalLength, std::vector<ModelTriangle> &sphereTriangles);
void gouraudShading (DrawingWindow &window, glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation, float &intensity,
                     const glm::vec3 &lightPosition, float focalLength, std::vector<ModelTriangle> &sphereTriangles);
void phongShading (DrawingWindow &window, glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation, float &intensity,
                   const glm::vec3 &lightPosition, float focalLength, const std::vector<ModelTriangle> &sphereTriangles);

enum shading {
    Flat,
    Gouraud,
    Phong
};

enum shadow {
    Hard,
    Soft,
    None
};

void rayTrace (DrawingWindow &window, glm::vec3 &cameraPosition, glm::mat3 cameraOrientation, glm::vec3 lightPosition,
               const std::vector<ModelTriangle> &completeTextModel, float &rotateAngle, float &intensity, shading shadingType, shadow shadowType);
#endif //SPHERE_H
