#include <UI/GameWindow.h>
#include <imgui.h>

void GameWindow::Render()
{
    ImGui::Begin("Game");
    ImGui::Text("Fenêtre du jeu... LETSSS GOOOOOOO");
    ImGui::End();
}