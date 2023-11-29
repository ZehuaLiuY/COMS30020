#include "sphere.h"

std::vector<ModelTriangle> readLogoFile(const std::string &filename, float scale) {
    std::ifstream objFile(filename);
    if (!objFile.is_open()) {
        std::cerr << "Failed to open the OBJ file: " << filename << std::endl;
        return {};
    }

    std::vector<ModelTriangle> modelTriangles;
    std::vector<glm::vec3> vertices;
    std::string line;

    while (std::getline(objFile, line)) {
        std::vector<std::string> values = split(line, ' ');
        if (values.empty()) continue;

        if (values[0] == "v") {
            try {
                vertices.push_back(scale * glm::vec3(std::stof(values[1]), std::stof(values[2]), std::stof(values[3])));
            } catch (const std::exception& e) {
                std::cerr << "Error parsing vertex: " << line << std::endl;
            }
        } else if (values[0] == "f") {
            try {
                glm::vec3 vertex1 = vertices[std::stoi(values[1]) - 1];
                glm::vec3 vertex2 = vertices[std::stoi(values[2]) - 1];
                glm::vec3 vertex3 = vertices[std::stoi(values[3]) - 1];
                Colour colour(255, 255, 255);
                ModelTriangle currentTriangle(vertex1, vertex2, vertex3, colour);
                currentTriangle.normal = getTriangleNormal(currentTriangle);
                modelTriangles.push_back(currentTriangle);
            } catch (const std::exception& e) {
                std::cerr << "Error parsing face: " << line << std::endl;
            }
        }
    }

    objFile.close();
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