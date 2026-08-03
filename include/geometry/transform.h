#ifndef LSIM_TRANSFORM_H
#define LSIM_TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Transform {
private:
    glm::vec3 position{0.0f};
    glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
    glm::vec3 scale{1.0f};
    glm::mat4 modelMatrix{1.0f};
    bool dirty = true;

    void applyTransformations();

public:
    void setPosition(const glm::vec3& p) { position = p; dirty = true; }
    void setRotation(const glm::quat& r) { rotation = r; dirty = true; }
    void setRotation(const glm::vec3& r) { setRotation(glm::quat(glm::radians(r))); }
    void setScale(const glm::vec3& s) { scale = s; dirty = true; }

    [[nodiscard]] const glm::vec3& getPosition() const { return position; }
    [[nodiscard]] const glm::quat& getRotation() const { return rotation; }
    [[nodiscard]] const glm::vec3& getScale() const { return scale; }
    [[nodiscard]] const glm::mat4& getModelMatrix() const { return modelMatrix; }

    void recomputeIfDirty();
};

#endif //LSIM_TRANSFORM_H
