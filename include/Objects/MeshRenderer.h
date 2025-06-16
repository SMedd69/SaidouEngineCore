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

        // Récupérer la liste de tous les matériaux disponibles
        auto materials = MaterialManager::Instance().GetAllMaterials();

        // Construire un vecteur de pointeurs vers les noms des matériaux (c_str() garantit la validité)
        std::vector<const char*> materialNames;
        materialNames.reserve(materials.size());
        for (const auto& mat : materials) {
            materialNames.push_back(mat->name.c_str());
        }

        // Trouver l'index du matériau actuellement sélectionné dans la liste
        int currentIndex = 0;
        for (size_t i = 0; i < materials.size(); ++i) {
            if (materials[i]->name == materialName) {
                currentIndex = static_cast<int>(i);
                break;
            }
        }

        // Afficher la combo box
        if (ImGui::Combo("Material", &currentIndex, materialNames.data(), static_cast<int>(materialNames.size()))) {
            // Mise à jour du matériau sélectionné
            materialName = materials[currentIndex]->name;
            material = materials[currentIndex];
        }

        if (material) {
            material->OnInspectorGUI();
        } else {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Material not found!");
        }
    }
};
