#include <UI/HierarchyWindow.h>
#include <imgui.h>

void HierarchyWindow::Render()
{
    ImGui::Begin("Hierarchy");
    ImGui::Text("Liste des entités...");
    ImGui::End();
}