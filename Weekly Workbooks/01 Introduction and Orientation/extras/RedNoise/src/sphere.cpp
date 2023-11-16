#include "wireframes.h"
#include "sphere.h"

std::vector<ModelTriangle> readSphereFile (const std::string &filename, float scale) {
    std::ifstream sphereFile(filename);
    if (!sphereFile.is_open()) {
        std::cerr << "Failed to open the OBJ file: " << filename << std::endl;
        return {};
    }

    std::vector<ModelTriangle> modelTriangles;
    std::string line;
    std::vector<glm::vec3> vertices;
    std::string index;

    // get each line of the file
    while (std::getline(sphereFile, line)) {
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
            Colour colour (0, 255, 255);
            ModelTriangle currentTriangle = ModelTriangle(vertex1, vertex2, vertex3, colour);
            currentTriangle.normal = getTriangleNormal(currentTriangle);
            modelTriangles.push_back(currentTriangle);
            // std::cout << "Assigned color for triangle: " << colour << std::endl;
        }
    }

    sphereFile.close();
    return modelTriangles;
}

void renderSphereWireframe(DrawingWindow &window, std::vector<std::pair<CanvasTriangle, Colour>> &sTriangles) {
    for (const auto& trianglePair : sTriangles) {
        CanvasTriangle triangle = trianglePair.first;
        Colour colour(255, 255, 255); // White Frame
        drawStrokedTriangle(window, triangle, colour);
    }
}

void renderSphereRasterised(DrawingWindow &window, std::vector<std::pair<CanvasTriangle, Colour>> &sTriangles) {
    for (const auto& trianglePair : sTriangles) {
        CanvasTriangle triangle = trianglePair.first;
        Colour fillColour = trianglePair.second;
        drawFilledTriangles(window, triangle, fillColour);
    }
}

// Task 6: Gouraud Shading

// use barycentric method
// reference https://en.wikipedia.org/wiki/Barycentric_coordinate_system
glm::vec3 getNormalWeight (float x, float y, const ModelTriangle &triangle) {
    glm::vec3 top = triangle.vertices[0];
    glm::vec3 middle = triangle.vertices[1];
    glm::vec3 bottom = triangle.vertices[2];

    float ratio = (middle.y - bottom.y) * (top.x - bottom.x) + (bottom.x - middle.x) * (top.y - bottom.y);
    float a = ((middle.y - bottom.y) * (x - bottom.x) + (bottom.x - middle.x) * (y - bottom.y)) / ratio;
    float b = ((bottom.y - top.y) * (x - bottom.x) + (top.x - bottom.x) * (y - bottom.y)) / ratio;
    float c = 1.0f - a - b;
    glm::vec3 normalWeight = glm::vec3(a, b, c);
    return normalWeight;
}

// get the normal of the vertex
glm::vec3 calculateVertexNormal (glm::vec3 vertex, const std::vector<ModelTriangle> &modelTriangles) {
    glm::vec3 normal = glm::vec3(0.0f, 0.0f, 0.0f);
    float i = 0.0f;
    for (const ModelTriangle &modelTriangle : modelTriangles) {
        std::array<glm::vec3, 3> vertices = modelTriangle.vertices;
        if (vertices[0] == vertex || vertices[1] == vertex || vertices[2] == vertex) {
            normal += modelTriangle.normal;
            i += 1.0f;
        }
    }
    glm::vec3 vertexNormal = normal / i;
    return glm::normalize(vertexNormal);
}

void gouraudShading (DrawingWindow &window, glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation,  const glm::vec3 &lightSource, float focalLength, std::vector<ModelTriangle> &sphereTriangles) {
    for (float h = 0; h < HEIGHT; h++) {
        for (float w = 0; w < WIDTH; w++) {
            glm::vec3 rayDirection = getDirection(cameraPosition, cameraOrientation, w, h, focalLength);
            RayTriangleIntersection ray = getClosestValidIntersection(cameraPosition, rayDirection, sphereTriangles);
            if (ray.distanceFromCamera != std::numeric_limits<float>::infinity()) {
                glm::vec3 lightDirection = glm::normalize(lightSource - ray.intersectionPoint);
                // get the normal weight
                glm::vec3 vertex1Normal = calculateVertexNormal(ray.intersectedTriangle.vertices[0], sphereTriangles);
                glm::vec3 vertex2Normal = calculateVertexNormal(ray.intersectedTriangle.vertices[1], sphereTriangles);
                glm::vec3 vertex3Normal = calculateVertexNormal(ray.intersectedTriangle.vertices[2], sphereTriangles);
                // adjust the brightness
                float brightness1 = processLighting(lightDirection, vertex1Normal, rayDirection);
                float brightness2 = processLighting(lightDirection, vertex2Normal, rayDirection);
                float brightness3 = processLighting(lightDirection, vertex3Normal, rayDirection);
                // find the weight of the normal
                glm::vec3 normalWeight = getNormalWeight(ray.intersectionPoint.x, ray.intersectionPoint.y, ray.intersectedTriangle);
                float brightness = normalWeight.x * brightness1 + normalWeight.y * brightness2 + normalWeight.z * brightness3;
                RayTriangleIntersection shadowRay = getClosestValidIntersection(ray.intersectionPoint + lightDirection * 0.001f, lightDirection, sphereTriangles);
                Colour colour = ray.intersectedTriangle.colour;
                Colour adjustedColour = Colour(colour.red * brightness, colour.green * brightness, colour.blue * brightness);
                window.setPixelColour(w, h, colourConverter(adjustedColour));
            }
        }
    }
}

// Task 7: Phong Shading
// get the normal of the triangle
glm::vec3 getTriangleNormalPoint (glm::vec3 trianglePoint1, glm::vec3 trianglePoint2, glm::vec3 trianglePoint3, glm::vec3 weight) {
    glm::vec3 normalPoint = weight[0] * trianglePoint1 + weight[1] * trianglePoint2 + weight[2] * trianglePoint3;
    return normalPoint;
}

void phongShading (DrawingWindow &window, glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation, const glm::vec3 &lightSource, float focalLength, std::vector<ModelTriangle> &sphereTriangles) {
for (float h = 0; h < HEIGHT; h++) {
        for (float w = 0; w < WIDTH; w++) {
            glm::vec3 rayDirection = getDirection(cameraPosition, cameraOrientation, w, h, focalLength);
            RayTriangleIntersection intersection = getClosestValidIntersection(cameraPosition, rayDirection, sphereTriangles);
            if (intersection.distanceFromCamera != std::numeric_limits<float>::infinity()) {
                glm::vec3 toLightDirection = glm::normalize(lightSource - intersection.intersectionPoint);
                // get the normal weight
                glm::vec3 vertex1Normal = calculateVertexNormal(intersection.intersectedTriangle.vertices[0], sphereTriangles);
                glm::vec3 vertex2Normal = calculateVertexNormal(intersection.intersectedTriangle.vertices[1], sphereTriangles);
                glm::vec3 vertex3Normal = calculateVertexNormal(intersection.intersectedTriangle.vertices[2], sphereTriangles);
                // find the weight of the normal
                glm::vec3 normalWeight = getNormalWeight(intersection.intersectionPoint.x, intersection.intersectionPoint.y, intersection.intersectedTriangle);
                // get the normal of the triangle
                glm::vec3 triangleNormal = getTriangleNormalPoint(vertex1Normal, vertex2Normal, vertex3Normal, normalWeight);
                float brightness = processLighting(toLightDirection, triangleNormal, rayDirection);
                RayTriangleIntersection shadowRay = getClosestValidIntersection(intersection.intersectionPoint + toLightDirection * 0.001f, toLightDirection, sphereTriangles);
                Colour colour = intersection.intersectedTriangle.colour;
                Colour adjustedColour = Colour(colour.red * brightness, colour.green * brightness, colour.blue * brightness);
                window.setPixelColour(w, h, colourConverter(adjustedColour));
            }
        }
    }
}