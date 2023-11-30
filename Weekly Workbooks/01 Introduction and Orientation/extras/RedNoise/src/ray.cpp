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

// multiple light sources
std::vector <glm::vec3> multipleLightSources (glm::vec3 LightPosition) {
    // make the lightPosition as the central light source
    std::vector <glm::vec3> lightSources;
    // store the central light source
    lightSources.push_back(LightPosition);
    int range = 10;
    float spread = 0.05f;
    for (int i = -range; i <= range; i++) {
        for (int k = -range; k <= range; k++) {
            if (std::abs(i) + std::abs(k) < range){
                glm::vec3 lightPoint = LightPosition + glm::vec3(i * spread, spread, k * spread);
                lightSources.push_back(lightPoint);
            }
        }
    }
    return lightSources;
}

// Task 2: closest intersection check the ray tracing is to be able to detect
RayTriangleIntersection getClosestValidIntersection(const glm::vec3 &cameraPosition, const glm::vec3 &rayDirection,
                                                    const std::vector<ModelTriangle> &modelTriangles) {
    RayTriangleIntersection closestIntersection;
    closestIntersection.distanceFromCamera = std::numeric_limits<float>::infinity();
    // for each triangle
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
        // determine the index of the triangle
        index++;
    }
    return closestIntersection;
}

glm::vec3 getDirection(glm::vec3 cameraPosition, glm::mat3 cameraOrientation, float x, float y, float focalLength) {
    float scale_u =(x - WIDTH / 2) / (160*focalLength);
    float scale_v =(y - HEIGHT / 2) / (160*focalLength);
    float z = cameraPosition.z - focalLength;
    // Assuming the image plane is one unit away from the camera, i.e., at z = -1 in camera space
    glm::vec3 pixelPosition = glm::vec3(scale_u + cameraPosition.x, -scale_v + cameraPosition.y, z) - cameraPosition; // z is negative because the camera looks along -z in right-handed coordinate system
    glm::vec3 rayDirection = glm::normalize(pixelPosition); // Normalize the direction vector
    rayDirection = rayDirection * inverse(cameraOrientation);
    return rayDirection;
}

// get the normal of each triangle call this function on readFiles
glm::vec3 getTriangleNormal (const ModelTriangle &modelTriangle) {
    glm::vec3 edge1 = modelTriangle.vertices[2] - modelTriangle.vertices[0];
    glm::vec3 edge2 = modelTriangle.vertices[1] - modelTriangle.vertices[0];
    glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
    return normal;
}

float processLighting(const glm::vec3 &lightDistance, const glm::vec3 &normal, glm::vec3 view) {
    // Normalize the light position vector
    glm::vec3 lightDirection = glm::normalize(lightDistance);

    // Proximity Lighting Calculation
    float pL = 12.0f / (4.0f * M_PI * glm::length(lightDistance) * glm::length(lightDistance));
    float proximity = std::min(std::max(pL, 0.00001f), 1.0f);

    // Angle-of-Incidence Lighting Calculation
    float angle = glm::dot(lightDirection, normal);
    float incidence = std::max(angle, 0.0f);

    // Specular Lighting Calculation
    // get the reflection vector, reference https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-shading/reflection-refraction-fresnel
    // R=L−2⋅(L⋅N)⋅N
    glm::vec3 reflectionVector = lightDirection - (2.0f * glm::dot(lightDirection, normal) * normal);
    float VR = glm::max (0.0f, glm::dot(reflectionVector, view));
    float specular = pow(VR, 256);

    float brightness = proximity * incidence + specular;

    // Ambient Lighting
    float ambientSupplement = 0.1f;
    brightness += ambientSupplement;

    // Minimum Brightness Threshold
    float ambientThreshold = 0.2f;
    brightness = std::max(brightness, ambientThreshold);

    // Clamping Brightness to the range [0.0, 1.0]
    return glm::clamp(brightness, 0.0f, 1.0f);
}
// process pixels with hard shadow
void processPixel(DrawingWindow &window, const glm::vec3 &cameraPosition, const glm::mat3 &cameraOrientation,
                  float x, float y, float focalLength, const std::vector<ModelTriangle> &modelTriangles, const glm::vec3 &lightPosition) {
    // from the camera to the pixel
    glm::vec3 rayToPixelDirection = getDirection(cameraPosition, cameraOrientation, x, y, focalLength);
    // get the closest triangle Intersection with the ray
    RayTriangleIntersection closestIntersection = getClosestValidIntersection(cameraPosition, rayToPixelDirection, modelTriangles);

    if (closestIntersection.distanceFromCamera != std::numeric_limits<float>::infinity()) {
        // from the closestIntersection point to the light source direction vector
        glm::vec3 lightDirection = glm::normalize(lightPosition - closestIntersection.intersectionPoint);
        // view is the direction from the closestIntersection point to the camera
        glm::vec3 view = glm::normalize(closestIntersection.intersectionPoint - cameraPosition);
        RayTriangleIntersection shadowRay = getClosestValidIntersection(closestIntersection.intersectionPoint + lightDirection * 0.001f, lightDirection, modelTriangles);

        float brightness;
        if (shadowRay.distanceFromCamera < glm::length(lightPosition - closestIntersection.intersectionPoint) && shadowRay.triangleIndex != closestIntersection.triangleIndex) {
            // in the shadow
            brightness = 0.2f;
        } else {
            // not in the shadow
            // lightDistance is the vector from the closestIntersection point to the light source
            glm::vec3 lightDistance = closestIntersection.intersectionPoint - lightPosition;
            // surface triangleNormal
            glm::vec3 triangleNormal = closestIntersection.intersectedTriangle.normal;
            brightness = processLighting(lightDistance, triangleNormal, view);
        }

        Colour colour = closestIntersection.intersectedTriangle.colour;
        uint32_t adjustedColour = colourConverter(Colour(colour.red * brightness, colour.green * brightness, colour.blue * brightness));
        window.setPixelColour(x, y, adjustedColour);
    } else {
        window.setPixelColour(x, y, colourConverter(Colour(0, 0, 0)));
    }
}


// Task 4: draw the ray tracing
void drawRayTracedScene (DrawingWindow &window, glm::vec3 &cameraPosition, glm::mat3 cameraOrientation,
                          float focalLength, const std::vector<ModelTriangle> &modelTriangles, glm::vec3 lightSource) {
    // from top to bottom
    for (float y = 0; y < HEIGHT; y++) {
        // from left to right
        for (float x = 0; x < WIDTH; x++) {
            processPixel(window, cameraPosition, cameraOrientation, x, y, focalLength, modelTriangles, lightSource);
        }
    }
}
// process pixels with soft shadow
void processPixelSoft(DrawingWindow &window, const glm::vec3 &cameraPosition, const glm::mat3 &cameraOrientation,
                      float x, float y, float focalLength, const std::vector<ModelTriangle> &modelTriangles, const std::vector<glm::vec3> &lightPositions) {
    // from the camera to the pixel
    glm::vec3 rayToPixelDirection = getDirection(cameraPosition, cameraOrientation, x, y, focalLength);
    // get the closest triangle Intersection with the ray
    RayTriangleIntersection closestIntersection = getClosestValidIntersection(cameraPosition, rayToPixelDirection, modelTriangles);

    if (closestIntersection.distanceFromCamera != std::numeric_limits<float>::infinity()) {
        // view is the direction from the closestIntersection point to the camera
        glm::vec3 view = glm::normalize(closestIntersection.intersectionPoint - cameraPosition);
        float numUnblocked = 0;

        for (const glm::vec3 &light : lightPositions){
            glm::vec3 lightDirection = glm::normalize(light - closestIntersection.intersectionPoint);
            // Nudging the start point to avoid self-intersection
            glm::vec3 startPoint = closestIntersection.intersectionPoint + lightDirection * 0.001f;
            RayTriangleIntersection shadowRay = getClosestValidIntersection(startPoint, lightDirection, modelTriangles);

            if (shadowRay.distanceFromCamera >= glm::length(light - closestIntersection.intersectionPoint) || shadowRay.triangleIndex == closestIntersection.triangleIndex) {
                numUnblocked += 1.0f;
            }
        }
        float shadowSoftness = glm::clamp<float>(numUnblocked / lightPositions.size(), 0.0f, 1.0f);

        // lightDistance is the vector from the closestIntersection point to the light source
        glm::vec3 lightDistance = closestIntersection.intersectionPoint - lightPositions[0];
        // surface triangleNormal
        glm::vec3 triangleNormal = getTriangleNormal(closestIntersection.intersectedTriangle);
        float brightness = processLighting(lightDistance, triangleNormal, view) * shadowSoftness;

        Colour colour = closestIntersection.intersectedTriangle.colour;
        uint32_t adjustedColour = colourConverter(Colour(colour.red * brightness, colour.green * brightness, colour.blue * brightness));
        window.setPixelColour(x, y, adjustedColour);
    } else {
        window.setPixelColour(x, y, colourConverter(Colour(0, 0, 0)));
    }
}


void drawRayTracedSceneSoft (DrawingWindow &window, glm::vec3 &cameraPosition, glm::mat3 cameraOrientation,
                         float focalLength, const std::vector<ModelTriangle> &modelTriangles, glm::vec3 lightSource) {
    std::vector<glm::vec3> lightPositions = multipleLightSources(lightSource);

    // from top to bottom
    for (float y = 0; y < HEIGHT; y++) {
        // from left to right
        for (float x = 0; x < WIDTH; x++) {
            processPixelSoft(window, cameraPosition, cameraOrientation, x, y, focalLength, modelTriangles, lightPositions);
        }
    }
}