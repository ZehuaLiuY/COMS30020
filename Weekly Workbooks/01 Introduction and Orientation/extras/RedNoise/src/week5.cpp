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
void rotateCameraByX(DrawingWindow &window, glm::vec3 &cameraPosition) {
    float angle = 0.1;
    glm::mat3 rotationMatrix(
            1, 0, 0,
            0, std::cos(angle), -std::sin(angle),
            0, std::sin(angle), std::cos(angle));
    cameraPosition = rotationMatrix * cameraPosition;
}

void rotateCameraByY(DrawingWindow &window, glm::vec3 &cameraPosition) {
    float angle = 0.1;
    glm::mat3 rotationMatrix(
            std::cos(angle), 0, std::sin(angle),
            0,  1, 0,
            -std::sin(angle), 0, std::cos(angle));
    cameraPosition = rotationMatrix * cameraPosition;
}

void lookAt(glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation) {
    glm::vec3 forward = glm::normalize(cameraPosition - glm::vec3(0, 0, 0));
    glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0, 1, 0), forward));
    glm::vec3 up = glm::normalize(glm::cross(forward, right));

//    glm::mat3 cameraOrientationMatrix = glm::mat3(
//            forward,
//            up,
//            right
//    );
//    cameraOrientation = cameraOrientationMatrix;
    cameraOrientation[0] = right;
    cameraOrientation[1] = up;
    cameraOrientation[2] = forward;
}

void rotateUp(DrawingWindow &window, glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation) {
    float angle = 0.1;
    glm::mat3 rotationMatrix(
            1, 0, 0,
            0, std::cos(angle), -std::sin(angle),
            0, std::sin(angle), std::cos(angle));
    cameraPosition = rotationMatrix * cameraPosition;
    lookAt(cameraPosition, cameraOrientation);
}

void rotateClock(DrawingWindow& window, glm::vec3& cameraPos, glm::mat3& cameraOrMat) {
    float angle = 0.1;
    glm::mat3 rotationMatrix(
            cos(angle), 0, sin(angle),
            0, 1, 0,
            -sin(angle),0, cos(angle));
    cameraPos = rotationMatrix * cameraPos;
    lookAt(cameraPos, cameraOrMat);
}