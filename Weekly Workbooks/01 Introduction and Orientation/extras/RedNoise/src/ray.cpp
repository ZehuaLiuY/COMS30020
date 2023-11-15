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
    glm::vec3 rayDirection = glm::vec3(scale_u + cameraPosition.x, -scale_v + cameraPosition.y, z) - cameraPosition; // z is negative because the camera looks along -z in right-handed coordinate system
    rayDirection = glm::normalize(rayDirection); // Normalize the direction vector
    rayDirection = rayDirection * inverse(cameraOrientation);
    return rayDirection;
}

// Week 7
// Task 2: proximity lighting 1.0 is the maximum brightness
float proximityLighting (float distance) {
    float pL = 30.0f / (4.0f* M_PI *distance*distance);
    return std::min(std::max(pL, 0.0f), 1.0f);
}

// Task 3: Angle-of-Incidence lighting
// get the normal of each triangle call this function on readFiles
glm::vec3 getTriangleNormal (const ModelTriangle &modelTriangle) {
    glm::vec3 edge1 = modelTriangle.vertices[2] - modelTriangle.vertices[0];
    glm::vec3 edge2 = modelTriangle.vertices[1] - modelTriangle.vertices[0];
    glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
    return normal;
}

float getIncidenceAngle (glm::vec3 lightDistance, glm::vec3 normal) {
    lightDistance = glm::normalize(lightDistance);
    float angle = glm::dot(lightDistance, normal);
    if (angle <= 0.0f) {
        angle = 0.0f;
    }
    return angle;
}

// Task 4: Specular Lighting
// get the reflection vector, reference https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-shading/reflection-refraction-fresnel
float specularLighting (glm::vec3 lightDistance, float incidenceAngle, glm::vec3 normal, glm::vec3 rayDirection) {
    // minus the incidence vector, because the incidence vector is pointing to the light source
    // R=L−2⋅(L⋅N)⋅N
    lightDistance = - glm::normalize(lightDistance);
    glm::vec3 reflection = lightDistance - 2.0f * glm::dot(lightDistance, normal) * normal;
    float VR = glm::max (0.0f, glm::dot(reflection, -rayDirection));
    float specular = pow(VR, 256);
    // get the specular lighting 1.0 is the maximum brightness
    specular = glm::clamp<float>(specular, 0.0f, 1.0f);
    return specular;
}

float processLighting(const glm::vec3 &intersectionPoint, const glm::vec3 &lightSource,
                      const ModelTriangle &intersectedTriangle, const glm::vec3 &rayDirection) {
    glm::vec3 lightDistance = intersectionPoint - lightSource;
    // Proximity lighting
    float proximity = proximityLighting(glm::length(lightDistance));
    // Angle-of-Incidence lighting
    float incidence = getIncidenceAngle(glm::normalize(lightDistance), intersectedTriangle.normal);
    // Specular Lighting
    float specular = specularLighting(lightDistance, incidence, intersectedTriangle.normal, rayDirection);

    float brightness = proximity * incidence + specular;

    // Ambient lighting
    // Universal supplement
    float ambientSupplement = 0.1f; // 0.1 is the ambient lighting
    brightness += ambientSupplement;

    // Minimum threshold
    float ambientThreshold = 0.2f;
    brightness = std::max(brightness, ambientThreshold);

    // Clamp the brightness to the range [0.0, 1.0]
    return glm::clamp(brightness, 0.0f, 1.0f);
}

void processPixel(DrawingWindow &window, const glm::vec3 &cameraPosition, const glm::mat3 &cameraOrientation,
                  float x, float y, float focalLength, const std::vector<ModelTriangle> &modelTriangles, const glm::vec3 &lightSource) {
    // calculate the ray direction
    glm::vec3 rayDirection = getDirection(cameraPosition, cameraOrientation, x, y, focalLength);
    RayTriangleIntersection ray = getClosestValidIntersection(cameraPosition, rayDirection, modelTriangles);

    if (ray.distanceFromCamera != std::numeric_limits<float>::infinity()) {
        // check if the intersection point is in the shadow
        glm::vec3 lightDirection = glm::normalize(lightSource - ray.intersectionPoint);
        RayTriangleIntersection shadowRay = getClosestValidIntersection(ray.intersectionPoint + lightDirection * 0.001f, lightDirection, modelTriangles);

        float brightness;
        if (shadowRay.distanceFromCamera < glm::length(lightSource - ray.intersectionPoint) && shadowRay.triangleIndex != ray.triangleIndex) {
            // in the shadow
            brightness = 0.2f;
        } else {
            brightness = processLighting(ray.intersectionPoint, lightSource, ray.intersectedTriangle, rayDirection);
        }

        Colour colour = ray.intersectedTriangle.colour;
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




// get the normal of the triangle
glm::vec3 getTriangleNormalPoint (glm::vec3 trianglePoint1, glm::vec3 trianglePoint2, glm::vec3 trianglePoint3, glm::vec3 weight) {
    glm::vec3 normalPoint = weight[0] * trianglePoint1 + weight[1] * trianglePoint2 + weight[2] * trianglePoint3;
    return normalPoint;
}

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
    return vertexNormal;
}

// use barycentric method
// reference https://en.wikipedia.org/wiki/Barycentric_coordinate_system
glm::vec3 getNormalWeight (float x, float y, const CanvasTriangle &triangle) {
    CanvasPoint top = triangle.vertices[0];
    CanvasPoint middle = triangle.vertices[1];
    CanvasPoint bottom = triangle.vertices[2];

    float ratio = (middle.y - bottom.y) * (top.x - bottom.x) + (bottom.x - middle.x) * (top.y - bottom.y);
    float a = ((middle.y - bottom.y) * (x - bottom.x) + (bottom.x - middle.x) * (y - bottom.y)) / ratio;
    float b = ((bottom.y - top.y) * (x - bottom.x) + (top.x - bottom.x) * (y - bottom.y)) / ratio;
    float c = 1.0f - a - b;
    glm::vec3 normalWeight = glm::vec3(a, b, c);
    return normalWeight;
}