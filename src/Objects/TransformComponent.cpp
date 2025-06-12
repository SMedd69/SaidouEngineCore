#include "Objects/TransformComponent.h"
#include <imgui.h>

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

    glm::decompose(matrix, scale, rotationQuat, position, skew, perspective);
    rotation = glm::degrees(glm::eulerAngles(rotationQuat)); // Rotation en degrés
}

glm::mat4 TransformComponent::GetTransformMatrix() const
{
    glm::mat4 t = glm::translate(glm::mat4(1.0f), position);
    glm::mat4 r = glm::toMat4(glm::quat(glm::radians(rotation)));
    glm::mat4 s = glm::scale(glm::mat4(1.0f), scale);
    return t * r * s;
}
