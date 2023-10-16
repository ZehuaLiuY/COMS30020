#ifndef WEEK4_H
#define WEEK4_H

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

std::vector<ModelTriangle> loadOBJ(std::string filename, std::map<std::string, Colour> palette, float scale);
std::map<std::string, Colour> loadMTL(std::string filename);
std::vector<ModelTriangle> readFiles(const std::string& objFilename, const std::string& mtlFilename, float scalingFactor);

#endif // WEEK4_H