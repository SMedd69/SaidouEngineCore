#pragma once
#include <string>
#include <fstream>
#include <filesystem>
#include <glm/vec4.hpp>  // glm::vec4
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
namespace fs = std::filesystem;



class Material {
public:
    std::string name = "Default";

    glm::vec4 albedo = glm::vec4(1.f, 1.f, 1.f, 1.f);  // RGBA
    float metallic = 0.0f;
    float roughness = 0.5f;
    float opacity = 1.0f;

    void OnInspectorGUI() {
        bool modified = false;

        char buf[128];
        std::strncpy(buf, name.c_str(), sizeof(buf));
        if (ImGui::InputText("Name", buf, sizeof(buf))) {
            name = std::string(buf);
            modified = true;
        }

        if (ImGui::ColorEdit4("Albedo", &albedo[0])) modified = true;
        if (ImGui::SliderFloat("Metallic", &metallic, 0.0f, 1.0f)) modified = true;
        if (ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f)) modified = true;
        if (ImGui::SliderFloat("Opacity", &opacity, 0.0f, 1.0f)) modified = true;

        if (modified) {
            SaveToFile();
        }
    }
    void SaveToFile() {
        // Exemple : le fichier se trouve dans Assets/<name>.mat
        fs::path matPath = "Assets/" + name + ".mat";
        std::ofstream out(matPath.string());
        if (out.is_open()) {
            out << "name=" << name << "\n";
            out << "albedo=" << albedo.r << "," << albedo.g << "," << albedo.b << "," << albedo.a << "\n";
            out << "metallic=" << metallic << "\n";
            out << "roughness=" << roughness << "\n";
            out << "opacity=" << opacity << "\n";
            out.close();
        }
    }

};
