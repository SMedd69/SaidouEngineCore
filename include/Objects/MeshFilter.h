#pragma once
#include <Objects/Component.h>
#include <imgui.h>
#include <string>

class MeshFilter : public Component {
public:
    std::string meshName = "Cube";

    std::string GetTypeName() const override {
        return "MeshFilter";
    }

    void OnInspectorGUI() override {
        ImGui::Text("Mesh Filter");
        ImGui::InputText("Mesh", &meshName[0], meshName.size() + 1);
    }
};
