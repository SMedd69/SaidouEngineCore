#include <UI/InspectorWindow.h>
#include <Objects/Component.h>
#include <Objects/MeshRenderer.h>
#include <Objects/BoxCollider.h>
#include <Objects/MeshFilter.h>
#include <imgui.h>

void InspectorWindow::Render()
{
    if (m_scene) DrawInspector(*m_scene);
}

void InspectorWindow::DrawInspector(Scene& scene) {
    ImGui::Begin("Inspector");

    if (m_selectedMaterial) {
        // Affiche l’inspecteur du Material sélectionné dans le projet
        m_selectedMaterial->OnInspectorGUI();

        // Désélectionne le matériau si on clique en dehors d’un contrôle
        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0)) {
            if (!ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive()) {
                m_selectedMaterial = nullptr;
            }
        }
    } 
    else if (scene.selectedObject) {
        // Affiche l’inspecteur du GameObject sélectionné dans la scène
        ImGui::Text("GameObject: %s", scene.selectedObject->name.c_str());

        for (auto& comp : scene.selectedObject->components) {
            comp->OnInspectorGUI();
        }

        if (ImGui::Button("Add Component")) {
            ImGui::OpenPopup("AddComponentPopup");
        }

        if (ImGui::BeginPopup("AddComponentPopup")) {
            if (ImGui::MenuItem("MeshRenderer")) {
                scene.selectedObject->AddComponent(std::make_shared<MeshRenderer>());
            }
            if (ImGui::MenuItem("BoxCollider")) {
                scene.selectedObject->AddComponent(std::make_shared<BoxCollider>());
            }
            if (ImGui::MenuItem("MeshFilter")) {
                scene.selectedObject->AddComponent(std::make_shared<MeshFilter>());
            }
            ImGui::EndPopup();
        }

        // Désélectionne le GameObject si on clique en dehors d’un contrôle
        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0)) {
            if (!ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive()) {
                scene.selectedObject = nullptr;
            }
        }
    } 
    else {
        ImGui::Text("No GameObject or Material selected");
    }

    ImGui::End();
}
