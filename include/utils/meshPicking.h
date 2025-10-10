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


    static bool RayIntersectsBoundingBox(const glm::vec3 &rayOrigin,
                                    const glm::vec3 &rayDir,
                                    const glm::vec3 &center,
                                    const glm::vec3 &size,
                                    float &outDistance) {
        const glm::vec3 halfSize(size.x / 2.0f, size.y / 2.0f, size.z / 2.0f);
        const glm::vec3 min = center - halfSize;
        const glm::vec3 max = center + halfSize;

        float tMin = (min.x - rayOrigin.x) / rayDir.x;
        float tMax = (max.x - rayOrigin.x) / rayDir.x;
        if (tMin > tMax) std::swap(tMin, tMax);

        float tyMin = (min.y - rayOrigin.y) / rayDir.y;
        float tyMax = (max.y - rayOrigin.y) / rayDir.y;
        if (tyMin > tyMax) std::swap(tyMin, tyMax);

        if ((tMin > tyMax) || (tyMin > tMax)) return false;

        tMin = std::max(tMin, tyMin);
        tMax = std::min(tMax, tyMax);

        float tzMin = (min.z - rayOrigin.z) / rayDir.z;
        float tzMax = (max.z - rayOrigin.z) / rayDir.z;
        if (tzMin > tzMax) std::swap(tzMin, tzMax);

        if ((tMin > tzMax) || (tzMin > tMax)) return false;

        tMin = std::max(tMin, tzMin);
        tMax = std::min(tMax, tzMax);

        outDistance = tMin >= 0.0f ? tMin : tMax;
        return outDistance >= 0.0f;
    }


    static int pickMesh(const std::vector<std::vector<std::unique_ptr<Mesh>>> &meshes,
                        const glm::vec3 &rayOrigin,
                        const glm::vec3 &rayDir,
                        const bool useSizeFunc = false,
                        const std::function<glm::vec3(Mesh*)> &sizeFunc = nullptr) {
        int selectedIndex = -1;
        float closetDistance = std::numeric_limits<float>::max();

         for (int i = 1; i < meshes.size(); ++i) {
             float distance;

             if (RayIntersectsBoundingBox(rayOrigin, rayDir, meshes[i][0]->position, useSizeFunc ? sizeFunc(meshes[i][0].get()) : meshes[i][0]->scale, distance)) {

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
