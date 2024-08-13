#include <glm/glm.hpp>
#include <vector>

glm::vec3 rotateVec(const glm::vec3 &input, const glm::vec3 &rotation);

std::vector<glm::vec3> spreadOrbit2d(const glm::vec3 center, const unsigned int numPoints,
                                     const float radius, const glm::vec2 range,
                                     const glm::vec3 rotation);

std::vector<glm::vec3> spreadOrbitRand(const glm::vec3 center, const int numPoints,
                                       const float radius, const glm::vec3 rotation);

std::vector<glm::vec3> spreadOrbitRand(const glm::vec3 center, const int numPoints,
                                       const float radius, const glm::vec2 thetaRange,
                                       const glm::vec2 phiRange, const glm::vec3 rotation);

std::vector<glm::vec3> spreadOrbit(const glm::vec3 center, const int numPoints, const float radius,
                                   const glm::vec3 rotation);

std::vector<glm::vec3> generatePointsOnSphericalSectorSurface(int numPoints, float radius,
                                                              float thetaMin, float thetaMax,
                                                              float phiMin, float phiMax);
