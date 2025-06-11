#include <UI/ProjectWindow.h>
#include <imgui.h>

void ProjectWindow::Render() {
    ImGui::Begin("Project");
    ImGui::Text("Liste des assets et scripts...");
    ImGui::End();
}