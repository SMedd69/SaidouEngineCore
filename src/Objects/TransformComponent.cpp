#include "Objects/TransformComponent.h"
#include "Objects/GameObject.h"
#include <imgui.h>
#include <iostream>

TransformComponent::TransformComponent() {
    position = {0, 0, 0};
    rotation = {0, 0, 0};
    scale = {1, 1, 1};
}

void TransformComponent::OnInspectorGUI() {
    ImGui::Text("Transform");
    ImGui::DragFloat3("Position", &position.x, 0.1f);
    ImGui::DragFloat3("Rotation", &rotation.x, 0.1f);
    ImGui::DragFloat3("Scale", &scale.x, 0.1f);
}

void TransformComponent::SetFromMatrix(const glm::mat4& matrix)
{
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::quat rotationQuat;

    bool success = glm::decompose(matrix, scale, rotationQuat, position, skew, perspective);

    if (!success) {
        std::cerr << "Warning: glm::decompose failed!" << std::endl;
    }

    rotation = glm::degrees(glm::eulerAngles(rotationQuat)); // Rotation en degrés

    std::cout << "SetFromMatrix position: " << position.x << ", " << position.y << ", " << position.z << std::endl;
    std::cout << "SetFromMatrix rotation: " << rotation.x << ", " << rotation.y << ", " << rotation.z << std::endl;
    std::cout << "SetFromMatrix scale: " << scale.x << ", " << scale.y << ", " << scale.z << std::endl;
}


glm::mat4 TransformComponent::GetTransformMatrix() const
{
    glm::mat4 t = glm::translate(glm::mat4(1.0f), position);
    glm::mat4 r = glm::toMat4(glm::quat(glm::radians(rotation)));
    glm::mat4 s = glm::scale(glm::mat4(1.0f), scale);
    return t * r * s;
}

glm::mat4 TransformComponent::GetWorldTransformMatrix() const
{
    glm::mat4 local = GetTransformMatrix();

    if (owner) {
        auto parentPtr = owner->parent.lock(); // ✅ Convertit le weak_ptr en shared_ptr
        if (parentPtr) {
            auto parentTransform = parentPtr->GetComponent<TransformComponent>();
            if (parentTransform) {
                return parentTransform->GetWorldTransformMatrix() * local;
            }
        }
    }

    return local;
}


