#include "sphere.h"

std::vector<ModelTriangle> readLogoFile(const std::string &filename, std::map<std::string, Colour> palette, float scale) {
    std::ifstream logoFile(filename);
    if (!logoFile.is_open()) {
        std::cerr << "Failed to open the OBJ file: " << filename << std::endl;
        return {};
    }

    std::string line;
    std::vector<glm::vec3> vertices;
    std::vector<ModelTriangle> modelTriangles;
    std::string materialName;
    std::vector<TexturePoint> texturePoints;

    while (std::getline(logoFile, line)) {
        std::vector<std::string> values = split(line, ' ');
        if (values[0] == "usemtl") {
            materialName = values[1];
        } else if (values[0] == "vt") {
            texturePoints.push_back(TexturePoint(std::stof(values[1]), std::stof(values[2])));
        } else if (values[0] == "v") {
            vertices.push_back(scale * glm::vec3(std::stof(values[1]), std::stof(values[2]), std::stof(values[3])));
        } else if (values[0] == "f") {
            glm::vec3 vertex[3];
            TexturePoint texturePoint[3];
            for (int i = 0; i < 3; ++i) {
                std::stringstream vertexStream(values[i + 1]);
                std::string vertexValue;
                int j = 0;
                while (std::getline(vertexStream, vertexValue, '/')) {
                    if (j == 0) vertex[i] = vertices[std::stoi(vertexValue) - 1];
                    else if (j == 1 && !vertexValue.empty()) {
                        texturePoint[i] = texturePoints[std::stoi(vertexValue) - 1];
                    }
                    j++;
                }
            }
            ModelTriangle currentTriangle(vertex[0], vertex[1], vertex[2], palette[materialName]);
            currentTriangle.texturePoints = {texturePoint[0], texturePoint[1], texturePoint[2]};
            currentTriangle.normal = getTriangleNormal(currentTriangle);
            currentTriangle.colour.name = "logo";
            modelTriangles.push_back(currentTriangle);
        }
    }

    logoFile.close();
    return modelTriangles;
}


std::vector<ModelTriangle> readLogoFiles(const std::string& logoFilename, const std::string& mtlFilename, float scalingFactor) {
    std::map<std::string, Colour> palette = loadMTL(mtlFilename);
    std::vector<ModelTriangle> modelTriangles = readLogoFile(logoFilename, palette, scalingFactor);
    return modelTriangles;
}

void renderLogoWireframe(DrawingWindow &window, std::vector<std::pair<CanvasTriangle, Colour>> &sTriangles) {
    for (const auto& trianglePair : sTriangles) {
        CanvasTriangle triangle = trianglePair.first;
        Colour colour(255, 255, 255); // White Frame
        drawStrokedTriangle(window, triangle, colour);
    }
}

void renderLogoRasterised(DrawingWindow &window, std::vector<std::pair<CanvasTriangle, Colour>> &sTriangles) {
    for (const auto& trianglePair : sTriangles) {
        CanvasTriangle triangle = trianglePair.first;
        Colour fillColour = trianglePair.second;
        drawFilledTriangles(window, triangle, fillColour);
    }
}