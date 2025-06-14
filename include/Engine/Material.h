#pragma once
#include <string>
#include <glm/vec4.hpp>  // glm::vec4
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

class Material {
public:
    std::string name = "Default";

    glm::vec4 albedo = glm::vec4(1.f, 1.f, 1.f, 1.f);  // RGBA
    float metallic = 0.0f;
    float roughness = 0.5f;
    float opacity = 1.0f;

    void OnInspectorGUI() {
        char buf[128];
        std::strncpy(buf, name.c_str(), sizeof(buf));
        if (ImGui::InputText("Name", buf, sizeof(buf))) {
            name = std::string(buf);
        }

        // ImGui attend un float* pour ColorEdit4, glm::value_ptr est pratique ici
        ImGui::ColorEdit4("Albedo", &albedo[0]);

        ImGui::SliderFloat("Metallic", &metallic, 0.0f, 1.0f);
        ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f);
        ImGui::SliderFloat("Opacity", &opacity, 0.0f, 1.0f);
    }
};
