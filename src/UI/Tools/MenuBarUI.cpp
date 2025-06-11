#include <UI/Tools/MenuBarUI.h>
#include <imgui.h>

void MenuBarUI::Render()
{
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Fichier")) {
            if (ImGui::MenuItem("Nouveau")) {
                // Action pour créer un nouveau fichier ou projet
            }
            if (ImGui::MenuItem("Ouvrir")) {
                // Action pour ouvrir un projet/fichier
            }
            if (ImGui::MenuItem("Sauvegarder")) {
                // Action pour sauvegarder
            }
            if(ImGui::MenuItem("Revenir au launcher"))
            {
                if(p_showLauncher)
                    *p_showLauncher = true;
            }
            if (ImGui::MenuItem("Quitter")) {
                // Action pour quitter
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Édition")) {
            if (ImGui::MenuItem("Annuler", "Ctrl+Z")) {
                // Action pour annuler
            }
            if (ImGui::MenuItem("Rétablir", "Ctrl+Y")) {
                // Action pour rétablir
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Aide")) {
            if (ImGui::MenuItem("À propos")) {
                // Afficher une popup ou une nouvelle fenêtre
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

}