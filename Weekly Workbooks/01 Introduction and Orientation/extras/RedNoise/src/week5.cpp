//
// Created by eclipse on 27/10/2023.
//
#include "week4.h"
#include "week5.h"

// translate the camera position in 3 dimensions (up/down, left/right, forwards/backwards)
void translateCamera(glm::vec3 &cameraPosition, float x, float y, float z) {
    cameraPosition.x += x;
    cameraPosition.y += y;
    cameraPosition.z += z;
}
// angle convertor
double deg2Rad(float degrees) {
    return degrees * M_PI / 180;
}

// rotate the camera position about the centre of the scene
// 3D  rotation about x, y, z axes

void rotateCameraByX(glm::vec3 &cameraPosition, float angle) {
    glm::mat3 rotationMatrix(
            1, 0, 0,
            0, std::cos(angle), -1 * std::sin(angle),
            0, std::sin(angle), std::cos(angle));
    //multiply the rotation mat3 and the camera position vec3 together to get the new camera position vec3
    cameraPosition = rotationMatrix * cameraPosition;
}

void rotateCameraByY(glm::vec3 &cameraPosition, float angle) {
    glm::mat3 rotationMatrix(
            std::cos(angle), 0, std::sin(angle),
            0, 1, 0,
            -1 * std::sin(angle), 0, std::cos(angle));

    cameraPosition = rotationMatrix * cameraPosition;
}
