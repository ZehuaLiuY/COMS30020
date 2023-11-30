#ifndef wireframes_h
#define wireframes_h

#define WIDTH 640
#define HEIGHT 480

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <vector>
#include "glm/glm.hpp"
#include <CanvasPoint.h>
#include <Colour.h>
#include <TextureMap.h>
#include <ModelTriangle.h>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

struct triangleData {
    CanvasTriangle triangle;
    Colour colour;
    int modelTriangleIndex;

    triangleData(const CanvasTriangle &tri, const Colour &col, int index)
            : triangle(tri), colour(col), modelTriangleIndex(index) {}
};

// Week 4 functions
// Task 2 & 3 read obj and mtl files
std::vector<ModelTriangle> loadOBJ(const std::string &filename, std::map<std::string, Colour> palette, float scale);
std::map<std::string, Colour> loadMTL(const std::string &filename);
std::vector<ModelTriangle> readFiles(const std::string& objFilename, const std::string& mtlFilename, float scalingFactor);
// Task 4 & 5: Projection
CanvasPoint getCanvasIntersectionPoint (glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation, glm::vec3 vertexPosition, float focalLength);
// Task 6: pointcloud render
void drawPoints(DrawingWindow &window, glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation, std::vector<ModelTriangle> modelTriangles, uint32_t colour);
// Task 7 & 8: Wireframe Render
std::vector<std::pair<CanvasTriangle, Colour>> triangleTransformer(const std::vector<ModelTriangle> &modelTriangles, glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation);

//Task 9 depth related function
float findDepth(float x, float y, CanvasTriangle triangle);
void resetDepthBuffer ();
void drawLineWithDepth(DrawingWindow &window, CanvasPoint from, CanvasPoint to, Colour colour, CanvasTriangle triangle);
void drawFilledTriangles(DrawingWindow &window, const CanvasTriangle &triangle, Colour &fillColour);
// render function for this week
void renderWireframe(DrawingWindow &window, std::vector<std::pair<CanvasTriangle, Colour>> &triangles);
void renderRasterised(DrawingWindow &window, std::vector<std::pair<CanvasTriangle, Colour>> &triangles);


// Week 5 functions
// translation camera
void translateCamera(glm::vec3 &cameraPosition, float x, float y, float z);
//rotation
void rotateCameraByX(glm::vec3 &cameraPosition);
void rotateCameraByY(glm::vec3 &cameraPosition);
// orbit
void lookAt(glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation);
void rotateUp(glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation, float angle);
void rotateClock(glm::vec3& cameraPos, glm::mat3& cameraOrMat, float angle);
void orbitClockwise(glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation, float step);
void orbitUp(glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation, float step);
void orbitSelf (glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation, float angle);

// Texture Mapping
std::vector<triangleData> processTriangles(const std::vector<ModelTriangle> &modelTriangles, glm::vec3 &cameraPosition, glm::mat3 &cameraOrientation);
void testProcess(DrawingWindow &window, std::vector<triangleData> &triangles);
void renderTexture(DrawingWindow &window, const std::vector<triangleData> &triangles, const std::vector<ModelTriangle> &modelTriangles);
#endif // wireframes_h