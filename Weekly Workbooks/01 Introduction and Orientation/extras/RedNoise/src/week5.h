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

void translateCamera(glm::vec3 &cameraPosition, float x, float y, float z);
// rotate matrix
void rotateCameraByX(glm::vec3 cameraPosition, float angle);
void rotateCameraByY(glm::vec3 cameraPosition, float angle);
void rotateCameraByZ(glm::vec3 cameraPosition, float angle);


#endif //WEEK5_H
