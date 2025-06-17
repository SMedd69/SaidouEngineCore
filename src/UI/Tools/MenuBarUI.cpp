#include <UI/Tools/MenuBarUI.h>
#include <imgui.h>
#include <filesystem>
#include <iostream>
#include <Engine/Renderer.h>
#include <Engine/Skybox.h>

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
            if (ImGui::MenuItem("Activer/Désactiver la grille")) {
                if(p_showGrid){
                    // Inverser l'état de la grille
                    if (*p_showGrid) {
                        // Si la grille est activée, on la désactive
                        *p_showGrid = false;
                        std::cout << "Grille désactivée" << std::endl;
                    } else {
                        // Si la grille est désactivée, on l'active
                        *p_showGrid = true;
                        std::cout << "Grille activée" << std::endl;
                    }
                }
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Environnement")) {
            // afficher un sous-menu pour changer l'environnement
            if (ImGui::BeginMenu("Changer l'environnement")) {
                // Dans se menu afficher la liste des fichiers png dans le dossier Assets/Skybox/default2
                auto skyboxFiles = Renderer::GetSkyboxFiles();
                for (const auto& file : skyboxFiles) {
                    if (ImGui::MenuItem(file.c_str())) {
                        Renderer::SetSkybox(std::make_shared<Skybox>(file, true));
                    }
                }
                ImGui::EndMenu();
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