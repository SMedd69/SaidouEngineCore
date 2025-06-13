#pragma once
#include "Objects/Component.h"
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

class TransformComponent : public Component {
public:
    glm::vec3 position, rotation, scale;

    TransformComponent();
    void SetFromMatrix(const glm::mat4& matrix);
    glm::mat4 GetTransformMatrix() const;
    glm::mat4 GetWorldTransformMatrix() const;

    std::string GetTypeName() const override { return "Transform"; }
    void OnInspectorGUI() override;
};
