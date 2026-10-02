#include <include/geometry/transform.h>

void Transform::recomputeIfDirty() {
    if (dirty) {
        applyTransformations();
        dirty = false;
    }
}

void Transform::applyTransformations() {
    modelMatrix = glm::translate(glm::mat4(1.0f), position)
             * glm::mat4_cast(rotation)
             * glm::scale(glm::mat4(1.0f), scale);
}
