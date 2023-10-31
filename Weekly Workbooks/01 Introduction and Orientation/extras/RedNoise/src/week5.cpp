//
// Created by eclipse on 27/10/2023.
//
#include "week4.h"
#include "week5.h"
//glm:vec3 initialCameraPosition() {
//    glm::vec3 cameraPosition =  glm::vec3(0, 0, 4.0);
//    return cameraPosition;
//}
//
//glm::mat3 initialCameraOrientation() {
//    glm::mat3 cameraOrientation = glm::mat3(
//            1, 0, 0,
//            0, 1, 0,
//            0, 0, 1
//    );
//    return cameraOrientation;
//}

// translate the camera position in 3 dimensions (up/down, left/right, forwards/backwards)
void translateCamera(glm::vec3 &cameraPosition, float x, float y, float z) {
    cameraPosition.x += x;
    cameraPosition.y += y;
    cameraPosition.z += z;
}
// angle convebianrtor
//float deg2Rad(float degrees) {
//    return degrees * M_PI / 180;
//}

// rotate the camera position about the centre of the scene
void rotateCameraByX(glm::vec3 &cameraPosition) {
    float angle = 0.01;
    glm::mat3 rotationMatrix(
            1, 0, 0,
            0, std::cos(angle), -std::sin(angle),
            0, std::sin(angle), std::cos(angle));
    cameraPosition = rotationMatrix * cameraPosition;
}

void rotateCameraByY(glm::vec3 &cameraPosition) {
    float angle = 0.01;
    glm::mat3 rotationMatrix(
            std::cos(angle), 0, std::sin(angle),
            0,  1, 0,
            -std::sin(angle), 0, std::cos(angle));
    cameraPosition = rotationMatrix * cameraPosition;
}



void rotateUp(glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation) {
    float angle = 0.01;
    glm::mat3 rotationMatrix(
            1, 0, 0,
            0, std::cos(angle), -std::sin(angle),
            0, std::sin(angle), std::cos(angle));
    cameraPosition = rotationMatrix * cameraPosition;
    lookAt(cameraPosition, cameraOrientation);
}

void rotateClock(glm::vec3& cameraPosition, glm::mat3& cameraOrientation) {
    float angle = 0.01;
    glm::mat3 rotationMatrix(
            cos(angle), 0, sin(angle),
            0, 1, 0,
            -sin(angle),0, cos(angle));
    cameraPosition = rotationMatrix * cameraPosition;
    lookAt(cameraPosition, cameraOrientation);
}

void lookAt(glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation) {
    glm::vec3 forward = glm::normalize(cameraPosition - glm::vec3(0, 0, 0));
    glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0, 1, 0), forward));
    glm::vec3 up = glm::normalize(glm::cross(forward, right));

    cameraOrientation[0] = right;
    cameraOrientation[1] = up;
    cameraOrientation[2] = forward;
}

void orbit(DrawingWindow &window, glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation) {
    window.clearPixels();
    resetDepthBuffer();
    float step = 0.0005;
    glm::mat3 orbit_rotation = glm::mat3(
            cos(step), 0, -sin(step),
            0, 1, 0,
            -sin(step),0, cos(step)
    );
    cameraPosition = orbit_rotation * cameraPosition;

    lookAt(cameraPosition, cameraOrientation);
}