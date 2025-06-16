#include <UI/ProjectWindow.h>
#include <Engine/MaterialManager.h>
#include <Engine/Material.h>
#include <imgui.h>

#include <iostream>
#include <fstream>

void ProjectWindow::Render() {
    ImGui::Begin("Project");
    ShowMenu();

    if (!m_projectPath.empty()) {
        float leftPanelWidth = 200.0f;

        ImGui::BeginChild("LeftPanel", ImVec2(leftPanelWidth, 0), true);
        RenderFolderTree(m_projectPath + "/Assets");  // Vue arborescente
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginChild("RightPanel", ImVec2(0, 0), true);
        RenderDirectory(m_selectedFolder);  // Contenu du dossier sélectionné
        ImGui::EndChild();
    }
    ImGui::End();
}

void ProjectWindow::ShowMenu(){
    if (ImGui::BeginPopupContextWindow("ProjectContext", ImGuiPopupFlags_MouseButtonRight)) {
        if (ImGui::MenuItem("Créer un dossier à la racine")) {
            m_contextFolderPath = m_projectPath + "/Assets";
            m_isCreatingFolder = true;
            m_renamingFolder = "NouveauDossier";
            std::filesystem::create_directory(m_contextFolderPath / m_renamingFolder);
        }
        if (ImGui::MenuItem("Créer un matériau")) {
            m_isCreatingMaterial = true;
            strcpy(m_newMaterialName, "NewMaterial");
            m_contextFolderPath = m_selectedFolder; // Créer dans dossier courant sélectionné
        }
        ImGui::EndPopup();
    }
}

void ProjectWindow::RenderDirectory(const std::filesystem::path& path) {
    using namespace std::filesystem;

    if (!exists(path) || !is_directory(path)) return;

    // Affichage InputText pour création matériau
    if (m_isCreatingMaterial) {
        ImGui::SetKeyboardFocusHere();
        if (ImGui::InputText("Nom du matériau", m_newMaterialName, sizeof(m_newMaterialName), ImGuiInputTextFlags_EnterReturnsTrue)) {
            // Valider la création
            std::string baseName = m_newMaterialName;
            std::string uniqueName = baseName;
            int index = 1;

            // Générer un nom unique en évitant les doublons
            while (exists(m_contextFolderPath / (uniqueName + ".mat"))) {
                uniqueName = baseName + "_" + std::to_string(index);
                ++index;
            }

            // Création du fichier .mat
            std::ofstream out(m_contextFolderPath / (uniqueName + ".mat"));
            if (out.is_open()) {
                out << "name=" << uniqueName << "\n";
                out << "albedo=1.0,1.0,1.0,1.0\n";
                out << "metallic=0.0\n";
                out << "roughness=0.5\n";
                out << "opacity=1.0\n";
                out.close();
            }

            // Ajouter le matériau au MaterialManager
            auto mat = std::make_shared<Material>();
            mat->name = uniqueName;
            MaterialManager::Instance().AddMaterial(mat);

            m_isCreatingMaterial = false;
        }

        ImGui::SameLine();
        if (ImGui::Button("Valider")) {
            // Même logique que la validation InputText (au cas où l'utilisateur préfère cliquer)
            std::string baseName = m_newMaterialName;
            std::string uniqueName = baseName;
            int index = 1;

            while (exists(m_contextFolderPath / (uniqueName + ".mat"))) {
                uniqueName = baseName + "_" + std::to_string(index);
                ++index;
            }

            std::ofstream out(m_contextFolderPath / (uniqueName + ".mat"));
            if (out.is_open()) {
                out << "name=" << uniqueName << "\n";
                out << "albedo=1.0,1.0,1.0,1.0\n";
                out << "metallic=0.0\n";
                out << "roughness=0.5\n";
                out << "opacity=1.0\n";
                out.close();
            }

            auto mat = std::make_shared<Material>();
            mat->name = uniqueName;
            MaterialManager::Instance().AddMaterial(mat);

            m_isCreatingMaterial = false;
        }

        ImGui::SameLine();
        if (ImGui::Button("Annuler")) {
            m_isCreatingMaterial = false;
        }
    }

    for (const auto& entry : directory_iterator(path)) {
        std::string name = entry.path().filename().string();

        if (m_isCreatingFolder && entry.path().filename() == m_renamingFolder) {
            char buffer[128];
            strncpy(buffer, name.c_str(), sizeof(buffer));
            buffer[sizeof(buffer) - 1] = '\0';

            if (ImGui::InputText("##RenameFolder", buffer, IM_ARRAYSIZE(buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
                std::string newName(buffer);
                rename(entry.path(), entry.path().parent_path() / newName);
                m_isCreatingFolder = false;
            }
            ImGui::SetKeyboardFocusHere(-1);
        } else {
            if (is_directory(entry)) {
                // Affiche les dossiers avec "/"
                if (ImGui::Selectable((name + "/").c_str())) {
                    m_selectedFolder = entry.path();
                    if (m_inspector) {
                        m_inspector->SetSelectedMaterial(nullptr);
                    }
                }
            } else {
                if (ImGui::Selectable(name.c_str())) {
                    if (entry.path().extension() == ".mat") {
                        auto mat = MaterialManager::Instance().GetMaterial(entry.path().stem().string());
                        if (mat && m_inspector) {
                            m_inspector->SetSelectedMaterial(mat);
                            if (m_inspector->m_scene) {
                                m_inspector->m_scene->selectedObject = nullptr;
                            }
                        }
                    }
                }
            }
        }
    }
}

void ProjectWindow::RenderFolderTree(const std::filesystem::path& path) {
    using namespace std::filesystem;

    if (!exists(path) || !is_directory(path)) return;

    for (const auto& entry : directory_iterator(path)) {
        if (!is_directory(entry)) continue;

        std::string name = entry.path().filename().string();
        bool open = ImGui::TreeNode(name.c_str());

        if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
            m_selectedFolder = entry.path();
        }

        if (ImGui::BeginPopupContextItem()) {
            m_contextFolderPath = entry.path();

            if (ImGui::MenuItem("Créer un dossier")) {
                m_isCreatingFolder = true;
                m_renamingFolder = "NouveauDossier";
                std::filesystem::create_directory(m_contextFolderPath / m_renamingFolder);
            }

            if (ImGui::MenuItem("Créer un matériau")) {
                // Ici, on ne crée plus directement, on active le mode création avec saisie
                m_isCreatingMaterial = true;
                strcpy(m_newMaterialName, "NewMaterial");
                m_contextFolderPath = entry.path();
            }

            ImGui::EndPopup();
        }

        if (open) {
            RenderFolderTree(entry.path());
            ImGui::TreePop();
        }
    }
}

void ProjectWindow::SetProjectPath(const std::string& path) {
    m_projectPath = path;
    m_selectedFolder = std::filesystem::path(path) / "Assets";
}
