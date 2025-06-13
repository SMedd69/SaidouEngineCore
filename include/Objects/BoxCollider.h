#pragma once
#include <Objects/Component.h>
#include <imgui.h>

class BoxCollider : public Component {
public:
    float size[3] = {1.0f, 1.0f, 1.0f};

    std::string GetTypeName() const override {
        return "BoxCollider";
    }

    void OnInspectorGUI() override {
        ImGui::Text("Box Collider");
        ImGui::DragFloat3("Size", size, 0.1f);
    }
};
