#include "wireframes.h"
#include "sphere.h"

glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
float radius = 0.0f;

// calculate the sphere center
glm::vec3 calculateSphereCenter (const std::vector<ModelTriangle> &sphereTriangles) {
    glm::vec3 c = glm::vec3(0.0f, 0.0f, 0.0f);
    for (const ModelTriangle &modelTriangle : sphereTriangles) {
        c += modelTriangle.vertices[0];
        c += modelTriangle.vertices[1];
        c += modelTriangle.vertices[2];
    }
    c = c / (float(sphereTriangles.size()) * 3.0f);
    return c;
}

// calculate the sphere radius
float calculateSphereRadius (const std::vector<ModelTriangle> &sphereTriangles, const glm::vec3 &center) {
    float r = 0.0f;
    for (const ModelTriangle &modelTriangle : sphereTriangles) {
        r = std::max(r, glm::length(modelTriangle.vertices[0] - center));
        r = std::max(r, glm::length(modelTriangle.vertices[1] - center));
        r = std::max(r, glm::length(modelTriangle.vertices[2] - center));
    }
    return r;
}

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
                Colour colour(0, 255, 255);
                ModelTriangle currentTriangle(vertex1, vertex2, vertex3, colour);
                currentTriangle.normal = getTriangleNormal(currentTriangle);
                currentTriangle.colour.name = "sphere";
                modelTriangles.push_back(currentTriangle);
            } catch (const std::exception& e) {
                std::cerr << "Error parsing face: " << line << std::endl;
            }
        }
    }
    center = calculateSphereCenter(modelTriangles);
    radius = calculateSphereRadius(modelTriangles, center);

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
                   const glm::vec3 &lightPosition, float focalLength, const std::vector<ModelTriangle> &sphereTriangles) {

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


// texture mapping
u_int32_t modelTextureMapping (TextureMap &texture, ModelTriangle triangle, glm::vec3 point) {
    glm::vec3 top = triangle.vertices[0];
    glm::vec3 middle = triangle.vertices[1];
    glm::vec3 bottom = triangle.vertices[2];

    float ratio = (middle.y - bottom.y) * (top.x - bottom.x) + (bottom.x - middle.x) * (top.y - bottom.y);
    float a = ((middle.y - bottom.y) * (point.x - bottom.x) + (bottom.x - middle.x) * (point.y - bottom.y)) / ratio;
    float b = ((bottom.y - top.y) * (point.x - bottom.x) + (top.x - bottom.x) * (point.y - bottom.y)) / ratio;
    float c = 1.0f - a - b;

    CanvasPoint texturePoint (round(a * triangle.texturePoints[0].x * float(texture.width) + b * triangle.texturePoints[1].x * float(texture.width) + c * triangle.texturePoints[2].x * float(texture.width)),
                              round(a * triangle.texturePoints[0].y * float(texture.height) + b * triangle.texturePoints[1].y * float(texture.height) + c * triangle.texturePoints[2].y * float(texture.height)));
    int index = int(texturePoint.x) + int(texturePoint.y) * texture.width;
    return texture.pixels[index - 1];
}


u_int32_t sphereTextureMapping(glm::vec3 point, TextureMap texture, float rotateAngle) {
    // Rotate the sphere
    glm::mat3 rotationYMatrix = glm::mat3{{cos(rotateAngle),  0, sin(rotateAngle)},
                                          {0,                 1, 0},
                                          {-sin(rotateAngle), 0, cos(rotateAngle)}};

    glm::vec3 translate = point - center;
    translate = rotationYMatrix * translate;
    glm::vec3 newIntersectionPoint = translate + center;
    glm::vec3 normalizedPoint = (newIntersectionPoint - center) / radius;

    // Map the texture
    float phi = atan2(normalizedPoint.y, normalizedPoint.x);
    float theta = acos(normalizedPoint.z);
    int i = phi / (2 * M_PI) * (texture.width);
    int j = theta / (M_PI) * (texture.height);
    int index = i + j * int(texture.width);
    return texture.pixels[index - 1];
}


glm::vec3 getRefractionDirection (glm::vec3 incident, glm::vec3 triangleNormal, float air, float glass) {
    float incidentAngle = glm::dot(incident, triangleNormal);
    float reflectiveRatio = air / glass;
    if (incidentAngle > 0) {
        reflectiveRatio = glass / air;
        triangleNormal = -triangleNormal;
    }
    //
    float k = 1 - reflectiveRatio * reflectiveRatio * (1 - abs(incidentAngle) * abs(incidentAngle));
    if (k < 0) {
        return {0, 0, 0};
    } else {
        return glm::normalize(incident * reflectiveRatio + triangleNormal * (reflectiveRatio * abs(incidentAngle) - sqrt(k)));
    }
}

float getRefractiveIndex (glm::vec3 incidenceRayDirection, float air, float glass, glm::vec3 normal) {

    float incidentAngle = glm::dot(incidenceRayDirection, normal);
    float refIndexRatio = air / glass;

    // incident ray is exiting the glass to air If incidentAngle is greater than zero
    if (incidentAngle > 0) refIndexRatio = glass / air;
    // refracted angle using Snell's Law
    // reference https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-shading/reflection-refraction-fresnel
    float refractionIndex = refIndexRatio * sqrtf(std::max(0.0f, 1 - incidentAngle * incidentAngle));
    // if refractionIndex is >=1, then it is a full internal reflection.
    if (refractionIndex < 1 ){
        incidentAngle = fabsf(incidentAngle);
        refractionIndex = sqrtf(std::max(0.0f, 1 - refractionIndex * refractionIndex));

        float Rs = ((glass * incidentAngle) - (glass * refractionIndex) / (air * refractionIndex) + (air * incidentAngle));

        float Rp = ((air * incidentAngle) - (glass * refractionIndex)) / ((air * incidentAngle) + (glass * refractionIndex));

        return 1 - (((glm::pow(Rs, 2.0f) + glm::pow(Rp, 2.0f)) )/ 2.0f);
    } else {
        return 0;
    }
}

Colour getRayIntesectedColour (glm::vec3 &cameraPosition, glm::vec3 &lightPosition, const std::vector<glm::vec3> &lightPositions,glm::vec3 &rayDirection,
                               const std::vector<ModelTriangle> &modelTriangles, float rotateAngle, int loopCount, shading shadingType, shadow shadowType) {
    if (loopCount > 10) {
        return {255, 255, 255};
    }
    // get the closest intersection with ray
    RayTriangleIntersection closestIntersection = getClosestValidIntersection(cameraPosition, rayDirection, modelTriangles);
    glm::vec3 point = closestIntersection.intersectionPoint;
    glm::vec3 v0 = closestIntersection.intersectedTriangle.vertices[0];
    glm::vec3 v1 = closestIntersection.intersectedTriangle.vertices[1];
    glm::vec3 v2 = closestIntersection.intersectedTriangle.vertices[2];
    glm::vec3 triangleNormal = closestIntersection.intersectedTriangle.normal;
    glm::vec3 view = glm::normalize(point - cameraPosition);
    Colour colour = closestIntersection.intersectedTriangle.colour;

    glm::vec3 lightDistance = point - lightPosition;
    glm::vec3 lightDirection = glm::normalize(lightDistance);

    std::string modelColour = closestIntersection.intersectedTriangle.colour.name;

    // texture mapping & reflection
    if(modelColour == "Blue"){
        glm::vec3 reflectedRay = glm::normalize(rayDirection - (2.0f * triangleNormal * glm::dot(rayDirection, triangleNormal)));
        Colour reflectionColour = getRayIntesectedColour(point, lightPosition, lightPositions, reflectedRay, modelTriangles, rotateAngle, loopCount + 1, shadingType, shadowType);

        glm::vec3 refractionDirection = getRefractionDirection(rayDirection, triangleNormal, 1.0f, 1.3f);
        // glm::vec3 newPoint = point + (triangleNormal * 0.001f);
        glm::vec3 newPoint;
        // if the ray is inside the glass, then the ray is exiting the glass else the ray is entering the glass
        if(glm::dot(rayDirection, triangleNormal) < 0 ) {
            newPoint = point - (triangleNormal * 0.0001f);
            // std::cout << "Ray inside the material, exiting. Offset point: " << newPoint.x << newPoint.y << std::endl;
        } else {
            newPoint = point + (triangleNormal * 0.0001f);
            // std::cout << "Ray outside the material, entering. Offset point: " << newPoint.x << newPoint.y << std::endl;
        }

        Colour refractionColour = getRayIntesectedColour(newPoint, lightPosition, lightPositions, refractionDirection, modelTriangles, rotateAngle, loopCount + 1, shadingType, shadowType);
        float refractiveIndex = getRefractiveIndex(rayDirection, 1.0f, 1.3f, triangleNormal);
        float reflectiveIndex = 1 - refractiveIndex;

        colour.red = int ((refractiveIndex * float(refractionColour.red) + reflectiveIndex * float(reflectionColour.red)));
        colour.green = int ((refractiveIndex * float(refractionColour.green) + reflectiveIndex * float(reflectionColour.green)));
        colour.blue = int ((refractiveIndex * float(refractionColour.blue) + reflectiveIndex * float(reflectionColour.blue)));
        return colour;
    }
    else if (modelColour == "Cobbles") {
        TextureMap texture = TextureMap("../src/files/texture.ppm");
        u_int32_t textureColour = modelTextureMapping(texture, closestIntersection.intersectedTriangle, point);
        // transfer u_int32_t to Colour
        colour.red = int ((textureColour >> 16) & 0xFF);
        colour.green = int ((textureColour >> 8) & 0xFF);
        colour.blue = int ((textureColour) & 0xFF);
    }
    else if (modelColour == "Yellow"){
        // reflective material
        glm::vec3 reflectionRay = glm::normalize(rayDirection -(2.0f * triangleNormal * glm::dot(rayDirection, triangleNormal)));
        return getRayIntesectedColour(point, lightPosition, lightPositions, reflectionRay, modelTriangles, rotateAngle, loopCount + 1, shadingType, shadowType);
    }
    else if (modelColour == "logo") {
        TextureMap texture = TextureMap("../src/files/textureLogo.ppm");
        u_int32_t logoColour = modelTextureMapping(texture, closestIntersection.intersectedTriangle, point);
        colour.red = int ((logoColour >> 16) & 0xFF);
        colour.green = int ((logoColour >> 8) & 0xFF);
        colour.blue = int ((logoColour) & 0xFF);

    }
    else if (modelColour == "sphere") {
        TextureMap sphereTexture = TextureMap("../src/files/Sandy.ppm");
        u_int32_t sphereTextureColour = sphereTextureMapping(point, sphereTexture, rotateAngle);
        colour.red = int ((sphereTextureColour >> 16) & 0xFF);
        colour.green = int ((sphereTextureColour >> 8) & 0xFF);
        colour.blue = int ((sphereTextureColour) & 0xFF);
   }

    // sphere shading
    float brightness = 0.0f;
    if (modelColour == "sphere" || shadingType == Phong || shadingType == Gouraud) {
        glm::vec3 vertex0Normal = calculateVertexNormal(v0, modelTriangles);
        glm::vec3 vertex1Normal = calculateVertexNormal(v1, modelTriangles);
        glm::vec3 vertex2Normal = calculateVertexNormal(v2, modelTriangles);
        // find the weight of the triangle
        glm::vec3 normalWeight = getNormalWeight(point.x, point.y, closestIntersection.intersectedTriangle);

        if (shadingType == Gouraud) {
            glm::vec3 lightDistance0 = v0 - lightPosition;
            glm::vec3 lightDistance1 = v1 - lightPosition;
            glm::vec3 lightDistance2 = v2 - lightPosition;

            glm::vec3 view0 = glm::normalize(v0 - cameraPosition);
            glm::vec3 view1 = glm::normalize(v1 - cameraPosition);
            glm::vec3 view2 = glm::normalize(v2 - cameraPosition);

            float brightness0 = processLighting(lightDistance0, vertex0Normal, view0);
            float brightness1 = processLighting(lightDistance1, vertex1Normal, view1);
            float brightness2 = processLighting(lightDistance2, vertex2Normal, view2);
            brightness = normalWeight.x * brightness0 + normalWeight.y * brightness1 + normalWeight.z * brightness2;
        }
        else if (modelColour == "sphere" ||shadingType == Phong) {
            glm::vec3 interpolatedNormal = glm::vec3 (normalWeight.x * vertex0Normal + normalWeight.y * vertex1Normal + normalWeight.z * vertex2Normal);
            brightness = processLighting(lightDistance, interpolatedNormal, view);
        }
    }
    else {
        brightness = processLighting(lightDistance, triangleNormal, view);
    }
    float red = std::min(255.0f, float(colour.red) * brightness);
    float green = std::min(255.0f, float(colour.green) * brightness);
    float blue = std::min(255.0f, float(colour.blue) * brightness);

    // shadow

    if (shadowType == Hard) {
        RayTriangleIntersection shadowRay = getClosestValidIntersection(point + lightDirection * 0.001f, lightDirection, modelTriangles);
        if (shadowRay.distanceFromCamera < glm::length( lightPosition - point) && shadowRay.triangleIndex != closestIntersection.triangleIndex && modelColour != "sphere" && modelColour != "Blue" ) {
            brightness = 0.2f;
        } else {
            brightness = processLighting(lightDistance, triangleNormal, view);
        }
        red = std::min(255.0f, float(colour.red) * brightness);
        green = std::min(255.0f, float(colour.green) * brightness);
        blue = std::min(255.0f, float(colour.blue) * brightness);

    }
    else if (shadowType == Soft) {
        float numUnblocked = 0;

        for (const glm::vec3 &light : lightPositions) {
            glm::vec3 softLightDirection = glm::normalize(light - point);
            glm::vec3 startPoint = point + softLightDirection * 0.001f;
            RayTriangleIntersection shadowRay = getClosestValidIntersection(startPoint, softLightDirection, modelTriangles);

            if ((shadowRay.distanceFromCamera >= glm::length(light - closestIntersection.intersectionPoint) || shadowRay.triangleIndex == closestIntersection.triangleIndex)) {
                numUnblocked += 1.0f;
            }
        }

        auto shadowSoftness = glm::clamp(numUnblocked / float(lightPositions.size()), 0.0f, 1.0f);
        brightness = processLighting(lightDistance, triangleNormal, view) * shadowSoftness;
        red = std::min(255.0f, float(colour.red) * brightness);
        green = std::min(255.0f, float(colour.green) * brightness);
        blue = std::min(255.0f, float(colour.blue) * brightness);

    } else if (shadowType == None) {
        return {int(red), int(green), int(blue)};
    }

    return {int(red), int(green), int(blue)};
}

//else if (modelColour == "Magenta") {
//glm::vec3 reflectionRay = glm::normalize(rayDirection -(2.0f * triangleNormal * glm::dot(rayDirection, triangleNormal)));
//return getRayIntesectedColour(point, lightPosition, lightPositions, reflectionRay, modelTriangles, loopCount + 1, shadingType, shadowType);


void rayTrace (DrawingWindow &window, glm::vec3 &cameraPosition, glm::mat3 cameraOrientation, glm::vec3 lightPosition, const std::vector<ModelTriangle> &completeModel, shading shadingType, shadow shadowType) {
    float rotateAngle = glm::radians(10.0f);
    int loopCount = 0;
    std::vector<glm::vec3> lightPositions = multipleLightSources(lightPosition);
    for (float h = 0; h < HEIGHT; h++) {
        for (float w = 0; w < WIDTH; w++) {
            glm::vec3 rayToPixelDirection = getDirection(cameraPosition, cameraOrientation, w, h, 2.0f);
            RayTriangleIntersection closestIntersection = getClosestValidIntersection(cameraPosition, rayToPixelDirection, completeModel);
            if (closestIntersection.distanceFromCamera != std::numeric_limits<float>::infinity()) {
                Colour colour = getRayIntesectedColour(cameraPosition, lightPosition, lightPositions, rayToPixelDirection, completeModel, rotateAngle, loopCount, shadingType, shadowType);
                window.setPixelColour(w, h, colourConverter(colour));
            } else {
                window.setPixelColour(w, h, colourConverter(Colour(0, 0, 0)));
            }
        }
    }
}