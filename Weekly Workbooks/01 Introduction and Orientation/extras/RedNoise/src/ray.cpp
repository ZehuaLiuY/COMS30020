#include "ray.h"

// Task 3: validation function
bool validation (const glm::vec3 possibleSolution) {
    float t = possibleSolution[0];
    float u = possibleSolution[1];
    float v = possibleSolution[2];

    if ((u >= 0.0 && u <= 1.0) && (v >= 0.0 && v <= 1.0) && (u + v <= 1.0) && t > 0.0) {
        return true;
    } else return false;
}

// Task 2: closest intersection check the ray tracing is to be able to detect
RayTriangleIntersection getClosestValidIntersection(const glm::vec3 &cameraPosition, const glm::vec3 &rayDirection,
                                                    const std::vector<ModelTriangle> &modelTriangles) {
    RayTriangleIntersection closestIntersection;
    closestIntersection.distanceFromCamera = std::numeric_limits<float>::infinity();
    int index = 0;
    for (const ModelTriangle &modelTriangle : modelTriangles) {
        std::array<glm::vec3, 3> vertices = modelTriangle.vertices;
        glm::vec3 e0 = vertices[1] - vertices[0];
        glm::vec3 e1 = vertices[2] - vertices[0];
        glm::vec3 SPVector = cameraPosition - vertices[0];
        glm::mat3 DEMatrix(-rayDirection, e0, e1);
        glm::vec3 possibleSolution = glm::inverse(DEMatrix) * SPVector;

        if (validation(possibleSolution) && possibleSolution[0] < closestIntersection.distanceFromCamera) {
            glm::vec3 r = vertices[0] + possibleSolution[1] * (vertices[1] - vertices[0]) + possibleSolution[2] * (vertices[2] - vertices[0]);
            closestIntersection = RayTriangleIntersection(r, possibleSolution[0], modelTriangle, index);
        }
    }
    return closestIntersection;
}

glm::vec3 getDirection(glm::vec3 cameraPosition, float x, float y, float focalLength) {

    float scale_u =(x - WIDTH / 2) / 180;
    float scale_v =(y - HEIGHT / 2) / 180;
    // Assuming the image plane is one unit away from the camera, i.e., at z = -1 in camera space
    glm::vec3 rayDirection = glm::vec3(scale_u, -scale_v, -focalLength); // z is negative because the camera looks along -z in right-handed coordinate system
    rayDirection = glm::normalize(rayDirection); // Normalize the direction vector
    return rayDirection;
}

// Task 4: draw the ray tracing
void drawRayTracedScene (DrawingWindow &window, glm::vec3 &cameraPosition,
                          float focalLength, const std::vector<ModelTriangle> modelTriangles){
    // from top to bottom
    for (float y = 0; y < HEIGHT; y++) {
        // from left to right
        for (float x = 0; x < WIDTH; x++) {
            // calculate the ray direction
            glm::vec3 rayDirection = getDirection(cameraPosition, x, y, focalLength);
            RayTriangleIntersection ray = getClosestValidIntersection(cameraPosition, rayDirection, modelTriangles);
            if (ray.distanceFromCamera != -1) {
                uint32_t colour = colourConverter(ray.intersectedTriangle.colour);
                // draw the pixel
                window.setPixelColour(x, y, colour);
            }
        }
    }
}
