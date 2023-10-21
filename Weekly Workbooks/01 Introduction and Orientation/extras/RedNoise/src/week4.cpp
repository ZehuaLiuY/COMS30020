#include "week4.h"
#include "week3.h"

// Task 2: read obj file
std::vector<ModelTriangle> loadOBJ(std::string filename, std::map<std::string, Colour> palette, float scale) {
    std::ifstream objFile(filename);
    if (!objFile.is_open()) {
        std::cerr << "Failed to open the OBJ file: " << filename << std::endl;
        return {};
    }

    std::string line;
    std::vector<glm::vec3> vertices;
    std::vector<ModelTriangle> modelTriangles;
    std::string index;

    // get each line of the file
    while (std::getline(objFile, line)) {
        std::vector<std::string> values = split(line, ' ');
        // if the line is a new vertex, add it to the vertices, if it's a new triangle, add it to the modelTriangles
        if (values[0] == "usemtl") {
            index = values[1];
        } else if (values[0] == "v") {
            vertices.push_back(scale * glm::vec3 (std::stof(values[1]), std::stof(values[2]), std::stof(values[3])));
        } else if (values[0] == "f") {
            glm::vec3 vertex1 = vertices[std::stoi(values[1].substr(0, values[1].find('/'))) - 1];
            glm::vec3 vertex2 = vertices[std::stoi(values[2].substr(0, values[2].find('/'))) - 1];
            glm::vec3 vertex3 = vertices[std::stoi(values[3].substr(0, values[3].find('/'))) - 1];
            modelTriangles.push_back(ModelTriangle(vertex1, vertex2, vertex3, palette[index]));
            // std::cout << "Assigned color for triangle: " << palette[index] << std::endl;

        }
    }

    objFile.close();
    return modelTriangles;
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
    std::vector<std::string> index;
    std::vector<Colour> colours;

    // get each line of the file
    while (std::getline(mtlFile, line)) {
        std::vector<std::string> values = split(line, ' ');
        // if the line is a new colour, add it to the palette
        if (values[0] == "newmtl") {
            index.push_back(values[1]);
        } else if (values[0] == "Kd") {
            Colour tempColour = Colour(std::stof(values[1]) * 255, std::stof(values[2]) * 255, std::stof(values[3]) * 255);
            colours.push_back(tempColour);
        }
    }
    // add the colours to the palette
    for (unsigned int i = 0; i < index.size(); i++) {
        palette[index[i]] = colours[i];
    }

    mtlFile.close();
    return palette;
}

// files reader wrapper
std::vector<ModelTriangle> readFiles(const std::string& objFilename, const std::string& mtlFilename, float scalingFactor) {
    std::map<std::string, Colour> palette = loadMTL(mtlFilename);
    std::vector<ModelTriangle> modelTriangles = loadOBJ(objFilename, palette, scalingFactor);

    return modelTriangles;
}

// Task 4 & 5 : render a graphical representation of the sample mode

// New function for return the 2D CanvasPoint position
CanvasPoint getCanvasIntersectionPoint (glm::vec3 cameraPosition, glm::vec3 vertexPosition, float focalLength) {
    glm::vec3 cameraCoordinate = vertexPosition - cameraPosition;
    // position on the image plane (ui, vi)
    // multiplier fot 160 looks good, if set to 240, many points will be out of scope
    float ui = focalLength * ((cameraCoordinate.x) / abs(cameraCoordinate.z)) * 160 + (WIDTH / 2);
    float vi = focalLength * ((cameraCoordinate.y) / abs(cameraCoordinate.z)) * 160 + (HEIGHT / 2);
    CanvasPoint intersectionPoint = CanvasPoint(ui, vi);
    return intersectionPoint;
}

// Task 6
void drawPoints(DrawingWindow &window, std::vector<ModelTriangle> modelTriangles, uint32_t colour) {
    glm::vec3 camaraPosition = glm::vec3 (0.0, 0.0, 4.0);
    float focalLength = 2.0;
    for (ModelTriangle modelTriangle : modelTriangles) {
        for (glm::vec3 points3d : modelTriangle.vertices) {
            CanvasPoint point = getCanvasIntersectionPoint(camaraPosition, points3d, focalLength);
            window.setPixelColour(round(point.x), round(point.y), colour);
        }
    }

}
