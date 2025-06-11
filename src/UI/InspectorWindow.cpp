#include <UI/InspectorWindow.h>
#include <imgui.h>

void InspectorWindow::Render()
{
    ImGui::Begin("Inspector");
    ImGui::Text("Composants de l'objet sélectionné...");
    ImGui::End();
}