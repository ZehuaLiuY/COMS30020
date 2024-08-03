#include "interpolation.h"


// Week2
std::vector<glm::vec3> interpolateThreeElementValues(const glm::vec3& from, const glm::vec3& to, int numberOfValues) {
    if (numberOfValues <= 1) {
        std::cerr << "Error: numberOfValues must be greater than 1." << std::endl;
        return {}; // Return an empty vector in case of error
    }

    std::vector<glm::vec3> result;
    glm::vec3 step = (to - from) / static_cast<float>(numberOfValues - 1);

    for (int i = 0; i < numberOfValues; ++i) {
        result.push_back(from + glm::vec3(i * step.x, i * step.y, i * step.z));
    }

    return result;
}

// return an evenly spaced list of size numberOfValues
std::vector<float> interpolateSingleFloats(float from, float to, int numberOfValues){

    if (numberOfValues <= 1) {
        std::cerr << "Error: numberOfValues must be greater than 1." << std::endl;
        return {}; // Return an empty vector in case of error
    }

    std::vector<float> result;
    float step = (to - from) / (float(numberOfValues) - 1);

    for (int i = 0; i < numberOfValues; ++i) {
        result.push_back(from + i * step);
    }

    return result;
}

void drawRGBColour(DrawingWindow &window){
    // create a vector of floats from 255 to 0 left to right
    std::vector<float> interpolationValues = interpolateSingleFloats(255.f, 0.0f, window.width);

    // create 4 corner values
    glm::vec3 topLeft(255, 0, 0);        // red
    glm::vec3 topRight(0, 0, 255);       // blue
    glm::vec3 bottomRight(0, 255, 0);    // green
    glm::vec3 bottomLeft(255, 255, 0);   // yellow

    // Create vectors
    std::vector<glm::vec3> interpolationValuesLeft = interpolateThreeElementValues(topLeft, bottomLeft, window.height);
    std::vector<glm::vec3> interpolationValuesRight = interpolateThreeElementValues(topRight, bottomRight, window.height);

    for (size_t y = 0; y < window.height; y++) {
        // Create a vector of interpolated RGB colors for the current row
        std::vector<glm::vec3> interpolationValuesRow = interpolateThreeElementValues(interpolationValuesLeft[y], interpolationValuesRight[y], window.width);

        for (size_t x = 0; x < window.width; x++) {
            glm::vec3 color = interpolationValuesRow[x];

            // Convert RGB color to uint32_t
            uint32_t red = static_cast<uint32_t>(color.r);
            uint32_t green = static_cast<uint32_t>(color.g);
            uint32_t blue = static_cast<uint32_t>(color.b);
            uint32_t alpha = 255;
            uint32_t pixelColor = (alpha << 24) + (red << 16) + (green << 8) + blue;

            window.setPixelColour(x, y, pixelColor);
        }
    }
}

