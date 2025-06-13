#pragma once
#include <Objects/Component.h>
#include <imgui.h>
#include <string>

class MeshRenderer : public Component {
public:
    std::string material = "Default";

    std::string GetTypeName() const override {
        return "MeshRenderer";
    }

    void OnInspectorGUI() override {
        ImGui::Text("Mesh Renderer");
        ImGui::InputText("Material", &material[0], material.size() + 1);
    }
};
