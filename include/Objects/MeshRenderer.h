#pragma once
#include <Objects/Component.h>
#include <Engine/Material.h>
#include <Engine/MaterialManager.h>
#include <imgui.h>
#include <string>

class MeshRenderer : public Component {
public:
    MeshRenderer() {
        material = MaterialManager::Instance().GetMaterial("Default");
        materialName = "Default";
    }
    std::shared_ptr<Material> material;


    std::string materialName = "Default";

    std::string GetTypeName() const override {
        return "MeshRenderer";
    }

    void OnInspectorGUI() override {
        ImGui::Text("Mesh Renderer");

        // Buffer pour InputText
        char buf[128];
        std::strncpy(buf, materialName.c_str(), sizeof(buf));

        if (ImGui::InputText("Material Name", buf, sizeof(buf))) {
            materialName = std::string(buf);
            material = MaterialManager::Instance().GetMaterial(materialName);
            if (!material) {
                ImGui::TextColored(ImVec4(1,0,0,1), "Material not found!");
            }
        }

        if (material) {
            material->OnInspectorGUI();
        } else {
            ImGui::TextColored(ImVec4(1,0,0,1), "Material not found!");
        }
    }

};
