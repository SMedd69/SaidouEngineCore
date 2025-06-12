#include <UI/InspectorWindow.h>
#include <Objects/Component.h>
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
    } else {
        ImGui::Text("No GameObject selected");
    }

    ImGui::End();
}