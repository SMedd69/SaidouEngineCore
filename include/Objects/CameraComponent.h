#pragma once
#include <Objects/Component.h>
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class CameraComponent : public Component {
public:
    float fov = 60.0f;
    float nearClip = 0.1f;
    float farClip = 100.0f;

    glm::mat4 GetProjectionMatrix(float aspect) const {
        return glm::perspective(glm::radians(fov), aspect, nearClip, farClip);
    }

    std::string GetTypeName() const override { return "Camera"; }
    void OnInspectorGUI() override {
        ImGui::Text("Camera");
        ImGui::DragFloat("FOV", &fov, 0.1f, 1.0f, 179.0f);
        ImGui::DragFloat("Near", &nearClip, 0.01f, 0.01f, 10.0f);
        ImGui::DragFloat("Far", &farClip, 1.0f, 1.0f, 1000.0f);
    }
};