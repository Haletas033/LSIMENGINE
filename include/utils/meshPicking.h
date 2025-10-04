//
// Created by halet on 10/5/2025.
//

#ifndef MESHPICKING_H
#define MESHPICKING_H


#include <memory>
#include <external/glm/glm.hpp>

#include "geometry/mesh.h"
#include "glm/ext/matrix_projection.hpp"

class meshPicking {
public:
    static glm::vec3 GetMouseRay(const float mouseX, const float mouseY,
                                 const glm::mat4& projection,
                                 const glm::mat4& view,
                                 const glm::vec4& viewport) {
        //Flip the Y
        const float flippedY = viewport.w - mouseY;

        //Unproject the nearPlane
        const glm::vec3 nearPoint = glm::unProject(
            glm::vec3(mouseX, flippedY, 0.0f),
            view,
            projection,
            viewport
        );

        //Unproject the farPlane
        const glm::vec3 farPoint = glm::unProject(
            glm::vec3(mouseX, flippedY, 1.0f),
            view,
            projection,
            viewport
        );

        return glm::normalize(farPoint - nearPoint);
    }


    static bool RayIntersectsSphere(const glm::vec3 &rayOrigin,
                                    const glm::vec3 &rayDir,
                                    const glm::vec3 &sphereCenter,
                                    const float sphereRadius,
                                    float &outDistance) {
        const glm::vec3 oc = rayOrigin - sphereCenter;
        const float b = glm::dot(oc, rayDir);
        const float c = glm::dot(oc, oc) - sphereRadius * sphereRadius;
        const float discriminant = b * b - c;

        if (discriminant < 0.0f) return false;

        outDistance = -b - sqrt(discriminant);
        if (outDistance < 0.0f) outDistance = -b + sqrt(discriminant);

        return outDistance >= 0.0f;
    }


    static int pickMesh(const std::vector<std::unique_ptr<Mesh>> &meshes,
                        const glm::vec3 &rayOrigin,
                        const glm::vec3 &rayDir) {
        int selectedIndex = -1;
        float closetDistance = std::numeric_limits<float>::max();

         for (int i = 0; i < meshes.size(); ++i) {
             float distance;

             if (RayIntersectsSphere(rayOrigin, rayDir, meshes[i]->position, 1.f, distance)) {

                 if (distance < closetDistance) {
                     selectedIndex = i;
                     closetDistance = distance;
                 }
             }
         }

        return selectedIndex;
    }
};

#endif //MESHPICKING_H
