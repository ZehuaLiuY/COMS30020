//
// Created by eclipse on 27/10/2023.
//
#ifndef WEEK5_H
#define WEEK5_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <vector>
#include "glm/glm.hpp"
#include <CanvasPoint.h>
#include <Colour.h>
#include <TextureMap.h>
#include <ModelTriangle.h>
#include <map>
#include "glm/vec3.hpp"
#include "glm/mat3x3.hpp"
#include "DrawingWindow.h"
//draw
CanvasPoint getCanvasIntersectionPointByOrientation(glm::vec3 cameraPosition , glm::mat3 cameraOrientation, glm::vec3 vertexPosition, float focalLength);
std::vector<std::pair<CanvasTriangle, Colour>> triangleTransformerWithOri(const std::vector<ModelTriangle> modelTriangles, glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation);
void renderWireframeWithORi(DrawingWindow &window, glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation );
//translate
void translateCamera(glm::vec3 &cameraPosition, float x, float y, float z);
//rotation
glm::vec3 initialCameraPosition();
glm::mat3 initialCameraOrientation();
void rotateCameraByX(glm::vec3 &cameraPosition);
void rotateCameraByY(glm::vec3 &cameraPosition);
void lookAt(glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation);
void rotateUp(glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation);
void rotateClock(glm::vec3& cameraPos, glm::mat3& cameraOrMat);

void orbit(DrawingWindow &window,glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation);


#endif //WEEK5_H
