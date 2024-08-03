#include "wireframes.h"
#include "triangle.h"
#include "interpolation.h"
#include "ray.h"

// Task 2: read obj file
std::vector<ModelTriangle> loadOBJ(const std::string &filename, std::map<std::string, Colour> palette, float scale) {
    std::ifstream objFile(filename);
    if (!objFile.is_open()) {
        std::cerr << "Failed to open the OBJ file: " << filename << std::endl;
        return {};
    }

    std::string line;
    std::vector<glm::vec3> vertices;
    std::vector<ModelTriangle> modelTriangles;
    std::string materialName;
    std::vector<TexturePoint> texturePoints;

    while (std::getline(objFile, line)) {
        std::vector<std::string> values = split(line, ' ');
        if (values.empty()) continue;

        if (values[0] == "usemtl") {
            materialName = values[1];
        } else if (values[0] == "vt") {
            texturePoints.emplace_back(std::stof(values[1]), std::stof(values[2]));
        } else if (values[0] == "v") {
            vertices.push_back(scale * glm::vec3(std::stod(values[1]), std::stod(values[2]), std::stod(values[3])));
        } else if (values[0] == "f") {
            glm::vec3 vertex1 = vertices[std::stoi(split(values[1], '/')[0]) - 1];
            glm::vec3 vertex2 = vertices[std::stoi(split(values[2], '/')[0]) - 1];
            glm::vec3 vertex3 = vertices[std::stoi(split(values[3], '/')[0]) - 1];
            ModelTriangle currentTriangle(vertex1, vertex2, vertex3, palette[materialName]);

            if (materialName == "Cobbles" && values.size() >= 4) {
                for (int i = 0; i < 3; ++i) {
                    std::vector<std::string> vertexData = split(values[i + 1], '/');
                    if (vertexData.size() > 1) {
                        int textureIndex = std::stoi(vertexData[1]) - 1;
                        if (textureIndex >= 0 && textureIndex < static_cast<int>(texturePoints.size())) {
                            currentTriangle.texturePoints[i] = texturePoints[textureIndex];
                        }
                    }
                }
            }

//            std::cout << "Texture Point 1: " << currentTriangle.texturePoints[0].x << ", " << currentTriangle.texturePoints[0].y << std::endl;
//            std::cout << "Texture Point 2: " << currentTriangle.texturePoints[1].x << ", " << currentTriangle.texturePoints[1].y << std::endl;
//            std::cout << "Texture Point 3: " << currentTriangle.texturePoints[2].x << ", " << currentTriangle.texturePoints[2].y << std::endl;

            currentTriangle.normal = getTriangleNormal(currentTriangle);
            currentTriangle.colour.name = materialName;
            modelTriangles.push_back(currentTriangle);
        }
    }

    objFile.close();
    return modelTriangles;
}



// Task 3: read mtl file
std::map<std::string, Colour> loadMTL(const std::string &filename) {
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
CanvasPoint getCanvasIntersectionPoint (glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation, glm::vec3 vertexPosition, float focalLength) {
    glm::vec3 cameraCoordinate = vertexPosition - cameraPosition;
    // update the cameraCoordinate by multiplying the cameraOrientation
    cameraCoordinate =  cameraCoordinate * cameraOrientation;
    // position on the image plane (ui, vi)
    float ui = round(focalLength * ((cameraCoordinate.x) / cameraCoordinate.z) * -320 + (WIDTH / 2));
    // top left corner is (0,0) to bottom right corner is (WIDTH, HEIGHT)
    float vi = round(focalLength * ((cameraCoordinate.y) / cameraCoordinate.z) * 320 + (HEIGHT / 2));
    float depthValue = abs(1 / cameraCoordinate.z); // Set the depth value
    CanvasPoint intersectionPoint = CanvasPoint(ui, vi, depthValue); // Pass the depth value to the CanvasPoint constructor
    return intersectionPoint;
}


// Task 6
void drawPoints(DrawingWindow &window, glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation, const std::vector<ModelTriangle> &modelTriangles, uint32_t colour) {
//    glm::vec3 cameraPosition = glm::vec3 (0.0, 0.0, 4.0);
    float focalLength = 2.0;
    for (const ModelTriangle &modelTriangle : modelTriangles) {
        for (glm::vec3 points3d : modelTriangle.vertices) {
            CanvasPoint point = getCanvasIntersectionPoint(cameraPosition, cameraOrientation, points3d, focalLength);
            window.setPixelColour(int(round(point.x)), int(round(point.y)), colour);
        }
    }
}

// Task 7: Wireframe Render
// function for transform 3D ModelTriangle to 2D CanvasTriangle
// add new attribute "colour" to each triangle
std::vector<std::pair<CanvasTriangle, Colour>> triangleTransformer(const std::vector<ModelTriangle> &modelTriangles, glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation) {
    std::vector<std::pair<CanvasTriangle, Colour>> canvasTriangles;

    float focalLength = 2.0;

    for (const ModelTriangle &modelTriangle : modelTriangles) {
        std::vector<CanvasPoint> canvasPoint;
        Colour colour = modelTriangle.colour;
        for (glm::vec3 points3d : modelTriangle.vertices) {
            CanvasPoint point = getCanvasIntersectionPoint(cameraPosition,cameraOrientation, points3d, focalLength);
            canvasPoint.push_back(point);
        }
        CanvasTriangle triangle = CanvasTriangle(canvasPoint[0], canvasPoint[1], canvasPoint[2]);
        canvasTriangles.emplace_back(triangle, colour);
    }

    return canvasTriangles;
}

// Task 9: find the weights of the point
std::vector<std::vector<float>> depthBuffer(HEIGHT, std::vector<float> (WIDTH, INT32_MIN));

// week 5
// function for reset the depth buffer to 0
void resetDepthBuffer () {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++){
            depthBuffer[i][j] = INT32_MIN;
        }
    }
}

// avoid some point out of the window
std::pair<CanvasPoint, CanvasPoint> clipLine(DrawingWindow &window, CanvasPoint from, CanvasPoint to) {
    int xMin = 0;
    int xMax = WIDTH - 1;
    int yMin = 0;
    int yMax = HEIGHT - 1;

    bool fromInside = (from.x >= xMin && from.x <= xMax && from.y >= yMin && from.y <= yMax);
    bool toInside = (to.x >= xMin && to.x <= xMax && to.y >= yMin && to.y <= yMax);

    if (fromInside && toInside) {
        return {from, to};
    }

    float m = (to.y - from.y) / (to.x - from.x);

    if (!fromInside) {
        if (from.x < xMin) {
            from.y += m * (xMin - from.x);
            from.x = xMin;
        } else if (from.x > xMax) {
            from.y += m * (xMax - from.x);
            from.x = xMax;
        }

        if (from.y < yMin) {
            from.x += (yMin - from.y) / m;
            from.y = yMin;
        } else if (from.y > yMax) {
            from.x += (yMax - from.y) / m;
            from.y = yMax;
        }
    }

    if (!toInside) {
        if (to.x < xMin) {
            to.y += m * (xMin - to.x);
            to.x = xMin;
        } else if (to.x > xMax) {
            to.y += m * (xMax - to.x);
            to.x = xMax;
        }

        if (to.y < yMin) {
            to.x += (yMin - to.y) / m;
            to.y = yMin;
        } else if (to.y > yMax) {
            to.x += (yMax - to.y) / m;
            to.y = yMax;
        }
    }

    return {from, to};
}


void drawLineWithDepth(DrawingWindow &window, CanvasPoint from, CanvasPoint to, Colour colour, CanvasTriangle triangle) {
//    auto clipped = clipLine(window, from, to);
//    from = clipped.first;
//    to = clipped.second;

    float xDiff = to.x - from.x;
    float yDiff = to.y - from.y;
    float numberOfSteps = std::max(abs(xDiff), abs(yDiff));
    float xStepSize = xDiff / numberOfSteps;
    float yStepSize = yDiff / numberOfSteps;
    float depthStepSize = (to.depth - from.depth) / numberOfSteps;
    uint32_t uIntColour = colourConverter(colour);
    for (float i = 0.0; i <= numberOfSteps; i++) {
        float x = from.x + (xStepSize * i);
        float y = from.y + (yStepSize * i);
        //float z = from.depth + (depthStepSize * float(i));
//        float z = findDepth(x, y, triangle);
        float z = from.depth + (depthStepSize * float(i));
        if (int(round(y)) < HEIGHT && int(round(y)) >= 0 && int(round(x)) < WIDTH && int(round(x)) >= 0){
            if (depthBuffer[int(round(y))][int(round(x))] < z) {
                depthBuffer[int(round(y))][int(round(x))] = z;
                window.setPixelColour(int(round(x)), int(round(y)), uIntColour);
            }
        }

    }
}

// Task 8 new function for drawing filled triangle, not the same as the one in week3.cpp
// using interpolation instead of slope
// new function for drawing filled triangle by interpolation
void drawFilledTriangles(DrawingWindow &window, const CanvasTriangle &triangle, Colour &fillColour) {
    std::array<CanvasPoint, 4> sortedPoints = calculateExtraPoint(triangle);

    CanvasPoint top = sortedPoints[0];
    CanvasPoint left = sortedPoints[1];
    CanvasPoint right = sortedPoints[2];
    CanvasPoint bottom = sortedPoints[3];

    if (left.x > right.x) {
        std::swap(left, right);
    }

    // Calculate the interpolation step for left and right edges of the top part of the triangle
    float topToMidY = abs(top.y - left.y);
    float xStepRT = (right.x - top.x) / topToMidY;
    float xStepLT = (left.x - top.x) / topToMidY;
    float depthStepRT = (right.depth - top.depth) / topToMidY;
    float depthStepLT = (left.depth - top.depth) / topToMidY;

    // Calculate the interpolation step for left and right edges of the bottom part of the triangle
    float midToBottomY = abs((bottom.y - left.y));
    float xStepBR = (bottom.x - right.x) / midToBottomY;
    float xStepBL = (bottom.x - left.x) / midToBottomY;
    float depthStepBR = (bottom.depth - right.depth) / midToBottomY;
    float depthStepBL = (bottom.depth - left.depth) / midToBottomY;

    // Draw the top part of the triangle
    for (float i = 0; i < topToMidY; i++) {
        CanvasPoint start = CanvasPoint(int(top.x + xStepRT * i), top.y + i, top.depth + depthStepRT * i);
        CanvasPoint end = CanvasPoint(int(top.x + xStepLT * i), top.y + i, top.depth + depthStepLT * i);
        drawLineWithDepth(window, start, end, fillColour, triangle);
    }
    // Draw the bottom part of the triangle
    for (float i = 0; i < midToBottomY; i++) {
        CanvasPoint start = CanvasPoint(int(right.x + xStepBR * i), right.y + i, right.depth + depthStepBR * i);
        CanvasPoint end = CanvasPoint(int(left.x + xStepBL * i), left.y + i, left.depth + depthStepBL * i);
        drawLineWithDepth(window, start, end, fillColour, triangle);
    }
}

// render the wireframe
void renderWireframe(DrawingWindow &window, std::vector<std::pair<CanvasTriangle, Colour>> &triangles) {
    for (const auto& trianglePair : triangles) {
        CanvasTriangle triangle = trianglePair.first;
        Colour colour(255, 255, 255); // White Frame
        drawStrokedTriangle(window, triangle, colour);
    }

}

void renderRasterised(DrawingWindow &window, std::vector<std::pair<CanvasTriangle, Colour>> &triangles) {
    for (const auto& trianglePair : triangles) {
        CanvasTriangle triangle = trianglePair.first;
        Colour fillColour = trianglePair.second;
        drawFilledTriangles(window, triangle, fillColour);
    }

}

// week 5 tasks: camera position and orientation
// translate the camera position in 3 dimensions (up/down, left/right, forwards/backwards)
void translateCamera(glm::vec3 &cameraPosition, float x, float y, float z) {
    cameraPosition.x += x;
    cameraPosition.y += y;
    cameraPosition.z += z;
}

// rotate the camera position about the centre of the scene
// rotate by x-axis (UP/DOWN)
void rotateCameraByX(glm::vec3 &cameraPosition) {
    float angle = 0.01;
    glm::mat3 rotationMatrix(
            1, 0, 0,
            0, std::cos(angle), -1 * std::sin(angle),
            0, std::sin(angle), std::cos(angle));
    cameraPosition = rotationMatrix * cameraPosition;
}

// rotate by y-axis (LEFT/RIGHT)
void rotateCameraByY(glm::vec3 &cameraPosition) {
    float angle = 0.01;
    glm::mat3 rotationMatrix(
            std::cos(angle), 0, std::sin(angle),
            0,  1, 0,
            -1 * std::sin(angle), 0, std::cos(angle));
    cameraPosition = rotationMatrix * cameraPosition;
}

void rotateUp(glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation, float angle) {
    glm::mat3 rotationMatrix(
            1, 0, 0,
            0, std::cos(angle), -1 * std::sin(angle),
            0, std::sin(angle), std::cos(angle));
    cameraPosition = rotationMatrix * cameraPosition;
    lookAt(cameraPosition, cameraOrientation);
}

void rotateClock(glm::vec3& cameraPosition, glm::mat3& cameraOrientation, float angle) {
    glm::mat3 rotationMatrix(
            std::cos(angle), 0, std::sin(angle),
            0,  1, 0,
            -1 * std::sin(angle), 0, std::cos(angle));
    cameraPosition = rotationMatrix * cameraPosition;
    lookAt(cameraPosition, cameraOrientation);
}

//void rotateSelf(glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation, float angle) {
//    glm::mat3 rotationMatrix(
//            std::cos(angle), -1 * std::sin(angle), 0,
//            std::sin(angle), std::cos(angle), 0,
//            0, 0, 1);
//    cameraPosition = rotationMatrix * cameraPosition;
//    lookAt(cameraPosition, cameraOrientation);
//}

void lookAt(glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation) {
    // (0,0,0) is the centre of the scene
    glm::vec3 forward = glm::normalize(cameraPosition - glm::vec3(0, 0, 0));
    glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0, 1, 0), forward));
    glm::vec3 up = glm::normalize(glm::cross(forward, right));

    cameraOrientation[0] = right;
    cameraOrientation[1] = up;
    cameraOrientation[2] = forward;
}

void orbitClockwise (glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation, float step) {
    rotateClock(cameraPosition, cameraOrientation, step);
}

void orbitUp (glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation, float step) {
    rotateUp(cameraPosition, cameraOrientation, step);
}

//void orbitSelf (glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation, float step) {
//    rotateSelf(cameraPosition, cameraOrientation, step);
//}

//void orbitClockwise (glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation, float angle) {
//    glm::quat rotation = glm::angleAxis(angle, glm::vec3(0, -1, 0));
//    cameraPosition = rotation * cameraPosition;
//    cameraOrientation = glm::mat3_cast(rotation) * cameraOrientation;
//}
//
//void orbitUp (glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation, float angle) {
//    glm::quat rotation = glm::angleAxis(angle, glm::vec3(-1, 0, 0));
//    cameraPosition = rotation * cameraPosition;
//    cameraOrientation = glm::mat3_cast(rotation) * cameraOrientation;
//}

void orbitSelf (glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation, float angle) {
    glm::quat rotation = glm::angleAxis(angle, glm::vec3(0, 0, -1));
    cameraPosition = rotation * cameraPosition;
    cameraOrientation = glm::mat3_cast(rotation) * cameraOrientation;
}

std::vector<triangleData> processTriangles(const std::vector<ModelTriangle> &modelTriangles, glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation) {
    std::vector<triangleData> canvasTriangles;

    float focalLength = 2.0;

    for (size_t i = 0; i < modelTriangles.size(); i++) {
        const ModelTriangle &modelTriangle = modelTriangles[i];
        std::vector<CanvasPoint> canvasPoints;

        for (const glm::vec3 &point3d : modelTriangle.vertices) {
            CanvasPoint canvasPoint = getCanvasIntersectionPoint(cameraPosition, cameraOrientation, point3d, focalLength);
            canvasPoints.push_back(canvasPoint);
        }

        CanvasTriangle canvasTriangle = CanvasTriangle(canvasPoints[0], canvasPoints[1], canvasPoints[2]);
        Colour colour = modelTriangle.colour;
        canvasTriangles.emplace_back(canvasTriangle, colour, i);  // 存储 CanvasTriangle、颜色和索引
    }

    return canvasTriangles;
}

void testProcess(DrawingWindow &window, std::vector<triangleData> &triangles) {
    for (const auto& triangle : triangles) {
        CanvasTriangle canvasTriangle = triangle.triangle;
        Colour fillColour = triangle.colour;
        drawFilledTriangles(window, canvasTriangle, fillColour);
        // std::cout << "Triangle " << triangle.modelTriangleIndex << " drawn" << std::endl;
    }
}
