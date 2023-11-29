#include "wireframes.h"
#include "sphere.h"

std::vector<ModelTriangle> readSphereFile (const std::string &filename, float scale) {
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
                Colour colour(255, 100, 0);
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

// Flat shading
void flatShading (DrawingWindow &window, glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation,
                  const glm::vec3 &lightPosition, float focalLength, std::vector<ModelTriangle> &sphereTriangles) {
    for (float h = 0; h < HEIGHT; h++) {
        for (float w = 0; w < WIDTH; w++) {
            // from the camera to the pixel
            glm::vec3 rayToPixelDirection = getDirection(cameraPosition, cameraOrientation, w, h, focalLength);
            // get the closest triangle Intersection with the closestIntersection
            RayTriangleIntersection closestIntersection = getClosestValidIntersection(cameraPosition, rayToPixelDirection, sphereTriangles);

            if (closestIntersection.distanceFromCamera != std::numeric_limits<float>::infinity()) {
                // from the closestIntersection point to the light source direction vector
                glm::vec3 lightDirection = lightPosition - closestIntersection.intersectionPoint;
                RayTriangleIntersection shadowRay = getClosestValidIntersection(closestIntersection.intersectionPoint + lightDirection * 0.001f, lightDirection, sphereTriangles);

                float brightness;
                if (shadowRay.distanceFromCamera < glm::length(lightPosition - closestIntersection.intersectionPoint) && shadowRay.triangleIndex != closestIntersection.triangleIndex) {
                    // in the shadow
                    brightness = 0.2f;
                } else {
                    // not in the shadow
                    glm::vec3 view = glm::normalize(closestIntersection.intersectionPoint - cameraPosition);
                    // lightDistance is the vector from the closestIntersection point to the light source
                    glm::vec3 lightDistance = closestIntersection.intersectionPoint - lightPosition;
                    // surface triangleNormal
                    glm::vec3 triangleNormal = closestIntersection.intersectedTriangle.normal;
                    brightness = processLighting(lightDistance, triangleNormal, view);
                }
                Colour colour = closestIntersection.intersectedTriangle.colour;
                uint32_t adjustedColour = colourConverter(Colour(colour.red * brightness, colour.green * brightness, colour.blue * brightness));
                window.setPixelColour(w, h, adjustedColour);
            } else {
                window.setPixelColour(w, h, colourConverter(Colour(0, 0, 0)));
            }
        }
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
    // std::cout << "normal weight: " << normalWeight.x << ", " << normalWeight.y << ", " << normalWeight.z << std::endl;
    return normalWeight;
}

// get the normal of the vertex
glm::vec3 calculateVertexNormal (glm::vec3 vertex, const std::vector<ModelTriangle> &sphereTriangles) {
    glm::vec3 normal = glm::vec3(0.0f, 0.0f, 0.0f);
    float i = 0.0f;
    for (const ModelTriangle &modelTriangle : sphereTriangles) {
        // std::array<glm::vec3, 3> vertices = modelTriangle.vertices;
        if (modelTriangle.vertices[0] == vertex || modelTriangle.vertices[1] == vertex || modelTriangle.vertices[2] == vertex) {
            normal += modelTriangle.normal;
            i ++; // add face number
        }
    }
    if (i != 0 ) {
        i = 1 / i;
    }
    glm::vec3 vertexNormal = normal * i;

    // use the average normal
//    std::cout << "Vertex normal for vertex at (" << vertex.x << ", " << vertex.y << ", " << vertex.z << "): "
//              << "Normal (" << vertexNormal.x << ", " << vertexNormal.y << ", " << vertexNormal.z << ")" << std::endl;
    return glm::normalize(vertexNormal);
}

void gouraudShading (DrawingWindow &window, glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation,
                     const glm::vec3 &lightPosition, float focalLength, std::vector<ModelTriangle> &sphereTriangles) {
    for (float h = 0; h < HEIGHT; h++) {
        for (float w = 0; w< WIDTH; w++){
            glm::vec3 rayToPixelDirection = getDirection(cameraPosition, cameraOrientation, w, h, focalLength);
            RayTriangleIntersection closestIntersection = getClosestValidIntersection(cameraPosition, rayToPixelDirection, sphereTriangles);
            if (closestIntersection.distanceFromCamera != std::numeric_limits<float>::infinity()) {
                glm::vec3 point = closestIntersection.intersectionPoint;
                // glm::vec3 lightDistance = lightPosition - closestIntersection.intersectionPoint;

                // for each vertex
                glm::vec3 v0 = closestIntersection.intersectedTriangle.vertices[0];
                glm::vec3 v1 = closestIntersection.intersectedTriangle.vertices[1];
                glm::vec3 v2 = closestIntersection.intersectedTriangle.vertices[2];

                //get each vertex lightDistance
                glm::vec3 lightDistance0 = v0 - lightPosition;
                glm::vec3 lightDistance1 = v1 - lightPosition;
                glm::vec3 lightDistance2 = v2 - lightPosition;

                // get the vertex normal
                glm::vec3 vertex0Normal = calculateVertexNormal(v0, sphereTriangles);
                glm::vec3 vertex1Normal = calculateVertexNormal(v1, sphereTriangles);
                glm::vec3 vertex2Normal = calculateVertexNormal(v2, sphereTriangles);
                // get the view vector
                glm::vec3 view0 = glm::normalize(v0 - cameraPosition);
                glm::vec3 view1 = glm::normalize(v1 - cameraPosition);
                glm::vec3 view2 = glm::normalize(v2 - cameraPosition);
                // get the brightness
                float brightness0 = processLighting(lightDistance0, vertex0Normal, view0);
                float brightness1 = processLighting(lightDistance1, vertex1Normal, view1);
                float brightness2 = processLighting(lightDistance2, vertex2Normal, view2);
                // find the weight of the triangle
                glm::vec3 normalWeight = getNormalWeight(point.x, point.y, closestIntersection.intersectedTriangle);
                // find the brightness of the pixel
                float brightness = normalWeight.x * brightness0 + normalWeight.y * brightness1 + normalWeight.z * brightness2;
                Colour colour = closestIntersection.intersectedTriangle.colour;
                Colour adjustedColour = Colour(colour.red * brightness, colour.green * brightness, colour.blue * brightness);
                window.setPixelColour(w, h, colourConverter(adjustedColour));
            }else{
                uint32_t black = colourConverter(Colour(0, 0, 0));
                window.setPixelColour(w, h, black);
            }
        }
    }
}

// Task 7: Phong Shading
// get the normal of the triangle
//glm::vec3 interpolateNormal (glm::vec3 vertex0Normal, glm::vec3 vertex1Normal,
//                             glm::vec3 vertex2Normal, glm::vec3 weight) {
//    glm::vec3 interpolatedNormal = weight[0] * vertex0Normal + weight[1] * vertex1Normal +
//                                   weight[2] * vertex2Normal;
//    // interpolatedNormal = glm::normalize(interpolatedNormal);
//    return interpolatedNormal;
//}

void phongShading (DrawingWindow &window, glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation,
                   const glm::vec3 &lightPosition, float focalLength, std::vector<ModelTriangle> &sphereTriangles) {

    for (float h = 0; h < HEIGHT; h++) {
        for (float w = 0; w < WIDTH; w++) {
            glm::vec3 rayToPixelDirection = getDirection(cameraPosition, cameraOrientation, w, h, focalLength);
            RayTriangleIntersection closestIntersection = getClosestValidIntersection(cameraPosition, rayToPixelDirection, sphereTriangles);
            if (closestIntersection.distanceFromCamera != std::numeric_limits<float>::infinity()) {
                glm::vec3 lightDistance = closestIntersection.intersectionPoint - lightPosition;
                glm::vec3 point = closestIntersection.intersectionPoint;

                // different from gouraud shading, phong shading interpolate the normal
                // for each vertex
                glm::vec3 v0 = closestIntersection.intersectedTriangle.vertices[0];
                glm::vec3 v1 = closestIntersection.intersectedTriangle.vertices[1];
                glm::vec3 v2 = closestIntersection.intersectedTriangle.vertices[2];

                // get the vertex normal
                glm::vec3 vertex0Normal = calculateVertexNormal(v0, sphereTriangles);
                glm::vec3 vertex1Normal = calculateVertexNormal(v1, sphereTriangles);
                glm::vec3 vertex2Normal = calculateVertexNormal(v2, sphereTriangles);
                // find the weight of the normal
                glm::vec3 normalWeight = getNormalWeight(point.x, point.y, closestIntersection.intersectedTriangle);
                glm::vec3 interpolatedNormal = glm::vec3 (normalWeight.x * vertex0Normal + normalWeight.y * vertex1Normal + normalWeight.z * vertex2Normal);

                glm::vec3 view = glm::normalize(point - cameraPosition);

                float brightness = processLighting(lightDistance, interpolatedNormal, view);

                Colour colour = closestIntersection.intersectedTriangle.colour;
                Colour adjustedColour = Colour(colour.red * brightness, colour.green * brightness, colour.blue * brightness);
                window.setPixelColour(w, h, colourConverter(adjustedColour));
            } else {
                window.setPixelColour(w, h, colourConverter(Colour(0, 0, 0)));
            }
        }
    }
}

