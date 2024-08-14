#include "pointMaths.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <json/json.h>
#include <random>
#include <stdlib.h>

glm::vec3 rotateVec(const glm::vec3 &input, const glm::vec3 &rotation) {
    const glm::mat4 rotationMat(1.0f);

    glm::mat4 rotationMatX = glm::rotate(rotationMat, rotation.x, glm::vec3(1.0, 0.0, 0.0));
    glm::mat4 rotationMatY = glm::rotate(rotationMat, rotation.y, glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 rotationMatZ = glm::rotate(rotationMat, rotation.z, glm::vec3(0.0, 0.0, 1.0));

    return glm::vec3(rotationMatX * rotationMatY * rotationMatZ * glm::vec4(input, 1.0));
}

std::vector<glm::vec3> spreadOrbit2d(const glm::vec3 center, const unsigned int numPoints,
                                     const float radius, const glm::vec2 range,
                                     const glm::vec3 rotation) {
    std::vector<glm::vec3> out(numPoints);
    const float angleDelta = ((range.y - range.x) / numPoints);

    for (int i = 0; i < numPoints; ++i) {
        float angle = range.x + angleDelta * i;

        glm::vec3 location(center.x + radius * cos(angle), center.y + radius * sin(angle),
                           center.z);

        out[i] = rotateVec(location, rotation);
    }

    return out;
}

std::vector<glm::vec3> spreadOrbitRand(const glm::vec3 center, const int numPoints,
                                       const float radius, const glm::vec3 rotation) {
    glm::vec2 thetaRange(0.0f, glm::pi<float>());
    glm::vec2 phiRange(0.0f, 2.0f * glm::pi<float>());
    return spreadOrbitRand(center, numPoints, radius, thetaRange, phiRange, rotation);
}

std::vector<glm::vec3> spreadOrbitRand(const glm::vec3 center, const int numPoints,
                                       const float radius, const glm::vec2 thetaRange,
                                       const glm::vec2 phiRange, const glm::vec3 rotation) {
    std::vector<glm::vec3> points;
    points.reserve(numPoints);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distPhi(phiRange.x, phiRange.y);
    std::uniform_real_distribution<float> distCosTheta(std::cos(thetaRange.y),
                                                       std::cos(thetaRange.x));

    for (int i = 0; i < numPoints; ++i) {
        float phi = distPhi(gen);
        float cosTheta = distCosTheta(gen);
        float theta = std::acos(cosTheta);

        float x = center.x + radius * sin(theta) * cos(phi);
        float y = center.y + radius * sin(theta) * sin(phi);
        float z = center.z + radius * cos(theta);

        points.push_back(rotateVec(glm::vec3(x, y, z), rotation));
        std::cout << x << " " << y << " " << z << std::endl;
    }

    return points;
}

std::vector<glm::vec3> generatePointsOnSphericalSectorSurface(int numPoints, float radius,
                                                              float thetaMin, float thetaMax,
                                                              float phiMin, float phiMax) {
    std::vector<glm::vec3> points;
    points.reserve(numPoints);

    // Random number generation
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distPhi(phiMin, phiMax);
    std::uniform_real_distribution<float> distCosTheta(std::cos(thetaMax), std::cos(thetaMin));

    for (int i = 0; i < numPoints; ++i) {
        // Randomly generate theta and phi within the given ranges
        float phi = distPhi(gen);
        float cosTheta = distCosTheta(gen);
        float theta = std::acos(cosTheta);

        // Convert spherical coordinates to Cartesian coordinates
        float x = radius * sin(theta) * cos(phi);
        float y = radius * sin(theta) * sin(phi);
        float z = radius * cos(theta);

        points.emplace_back(x, y, z);
    }

    return points;
}

std::vector<glm::vec3> spreadOrbit(const glm::vec3 center, const int numPoints, const float radius,
                                   const glm::vec3 rotation) {
    std::vector<glm::vec3> out;
    out.reserve(numPoints);

    const float phi = glm::pi<float>() * (sqrt(5.0f) - 1);

    for (int i = 0; i < numPoints; ++i) {
        float z = 1.0 - (static_cast<float>(i) / (numPoints - 1.0f)) * 2.0f;
        float unitRadius = sqrt(1.0f - z * z);

        float theta = phi * i;

        float x = center.x + cos(theta) * unitRadius * radius;
        float y = center.y + sin(theta) * unitRadius * radius;

        out.push_back(rotateVec(glm::vec3(x, y, center.z + z * radius), rotation));
    }

    return out;
}