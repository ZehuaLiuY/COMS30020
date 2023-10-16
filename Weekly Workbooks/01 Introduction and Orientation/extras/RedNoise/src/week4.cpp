#include "week4.h"

// Task 2: read obj file
std::vector<ModelTriangle> loadOBJ(std::string filename, std::map<std::string, Colour> palette, float scale) {
    std::ifstream objFile(filename);
    if (!objFile.is_open()) {
        std::cerr << "Failed to open the OBJ file: " << filename << std::endl;
        return {};
    }

    std::string line;
    std::vector<glm::vec3> vertices;
    std::vector<ModelTriangle> triangles;
    std::string name;
    // get each line of the file
    while (std::getline(objFile, line)) {
        std::vector<std::string> values = split(line, ' ');
        if (values[0] == "usemtl") {
            name = values[1];
        } else if (values[0] == "v") {
            vertices.push_back(scale * glm::vec3 (std::stof(values[1]), std::stof(values[2]), std::stof(values[3])));
        } else if (values[0] == "f") {
            glm::vec3 vertex1 = vertices[values[1][0] - '0' - 1]; // - '0' is how you convert char to int
            glm::vec3 vertex2 = vertices[values[2][0] - '0' - 1];
            glm::vec3 vertex3 = vertices[values[3][0] - '0' - 1];
            triangles.push_back(ModelTriangle(vertex1, vertex2, vertex3, palette[name]));
        }
    }

    objFile.close();
    return triangles;
}

// Task 3: read mtl file
std::map<std::string, Colour> loadMTL(std::string filename) {
    std::ifstream mtlFile(filename);
    if (!mtlFile.is_open()) {
        std::cerr << "Failed to open the OBJ file: " << filename << std::endl;
        return {};
    }

    std::string line;
    std::map<std::string, Colour> palette;
    std::vector<std::string> names;
    std::vector<Colour> colours;

    while (std::getline(mtlFile, line)) {
        std::vector<std::string> values = split(line, ' ');
        if (values[0] == "newmtl") {
            names.push_back(values[1]);
        } else if (values[0] == "Kd") {
            Colour tempColour = Colour(std::stof(values[1]) * 255, std::stof(values[2]) * 255, std::stof(values[3]) * 255);
            colours.push_back(tempColour);
        }
    }
    for (unsigned int i = 0; i < names.size(); i++) {
        palette[names[i]] = colours[i];
    }
    mtlFile.close();
    return palette;
}

// files reader wrapper
std::vector<ModelTriangle> readFiles(const std::string& objFilename, const std::string& mtlFilename, float scalingFactor) {
    std::map<std::string, Colour> palette = loadMTL(mtlFilename);
    std::vector<ModelTriangle> triangles = loadOBJ(objFilename, palette, scalingFactor);

    return triangles;
}
