// 
#ifndef LOGO_H
#define LOGO_H
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include "glm/glm.hpp"
#include <CanvasPoint.h>
#include <Colour.h>
#include <TextureMap.h>
#include <ModelTriangle.h>
#include <map>
#include <sstream>

#include "wireframes.h"

std::vector<ModelTriangle> readLogoFile(const std::string &filename, float scale);

void renderLogoWireframe(DrawingWindow &window, std::vector<std::pair<CanvasTriangle, Colour>> &sTriangles);
void renderLogoRasterised(DrawingWindow &window, std::vector<std::pair<CanvasTriangle, Colour>> &sTriangles);
#endif //LOGO_H
