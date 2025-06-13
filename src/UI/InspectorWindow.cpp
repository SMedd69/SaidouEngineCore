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

    if (scene.selectedObject) {
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
    } else {
        ImGui::Text("No GameObject selected");
    }

    ImGui::End();
}