#include <UI/ProjectWindow.h>
#include <Engine/MaterialManager.h>
#include <Engine/ScriptManager.h>
#include <UI/Tools/IconManager.h>
#include <Engine/Material.h>
#include <imgui.h>
#include <filesystem>
#include <iostream>
#include <fstream>

namespace fs = std::filesystem;

static float s_leftPanelWidth = 220.0f;
static float s_minPanelWidth = 120.0f;
static float s_iconSize = 64.0f;
static float s_minRightPanelWidth = 200.0f; // largeur minimale du panneau droit

void ProjectWindow::ShowContextMenu(const fs::path& targetFolder, const fs::path& targetItem, bool isFolder, bool isOnItem) {
    if (isOnItem && ImGui::MenuItem("Renommer")) {
        m_isRenaming = true;
        m_renamingPath = targetItem;
        strncpy(m_renamingBuffer, targetItem.filename().string().c_str(), sizeof(m_renamingBuffer));
        m_renamingBuffer[sizeof(m_renamingBuffer) - 1] = '\0';
    }
    if (isOnItem && ImGui::MenuItem("Supprimer")) {
        if (IsPathInProject(targetItem)) {
            if (isFolder)
                fs::remove_all(targetItem);
            else
                fs::remove(targetItem);
        }
    }
    if (ImGui::MenuItem("Créer un dossier")) {
        // NE PAS créer le dossier ici !
        m_isCreatingFolder = true;
        m_contextFolderPath = targetFolder;
        m_renamingFolder = "NouveauDossier";
    }
    if (ImGui::MenuItem("Nouveau material")) {
        m_isCreatingMaterial = true;
        strcpy(m_newMaterialName, "NewMaterial");
        m_contextFolderPath = targetFolder;
    }
    if (ImGui::MenuItem("Nouveau script")) {
        m_isCreatingScript = true;
        strcpy(m_newScriptName, "NewScript");
        m_contextFolderPath = targetFolder;
    }
}

void ProjectWindow::RenderFolderTree(const fs::path& path) {
    if (!fs::exists(path) || !fs::is_directory(path)) return;
    for (const auto& entry : fs::directory_iterator(path)) {
        if (!entry.is_directory()) continue;
        std::string name = entry.path().filename().string();
        ImGuiTreeNodeFlags flags = (m_selectedFolder == entry.path() ? ImGuiTreeNodeFlags_Selected : 0)
                                    | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
        bool open = ImGui::TreeNodeEx(name.c_str(), flags);

        if (ImGui::IsItemClicked()) {
            m_selectedFolder = entry.path();
        }
        if (ImGui::BeginPopupContextItem()) {
            ShowContextMenu(entry.path(), entry.path(), true, true);
            ImGui::EndPopup();
        }
        if (open) {
            RenderFolderTree(entry.path());
            ImGui::TreePop();
        }
    }
}

void ProjectWindow::RenderDirectory(const fs::path& folder) {
    if (!fs::exists(folder) || !fs::is_directory(folder)) return;

    ImGui::SliderFloat("Zoom", &s_iconSize, 16.0f, 128.0f, "%.0f px");
    bool iconMode = s_iconSize >= 32.0f;
    if (iconMode) {
        float panelWidth = ImGui::GetContentRegionAvail().x;
        int columnCount = std::max(1, int(panelWidth / (s_iconSize + 32.0f)));
        ImGui::Columns(columnCount, nullptr, false);
    }

    bool renameDone = false;

    // Affiche dossiers d'abord
    for (const auto& entry : fs::directory_iterator(folder)) {
        if (!entry.is_directory()) continue;
        std::string name = entry.path().filename().string();

        if (iconMode) {
            ImGui::BeginGroup();
            // Renommage
            if (m_isRenaming && entry.path() == m_renamingPath) {
                ImGui::PushID(name.c_str());
                ImGui::SetKeyboardFocusHere();
                if (ImGui::InputText("##Rename", m_renamingBuffer, sizeof(m_renamingBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
                    std::string newName(m_renamingBuffer);
                    auto newPath = entry.path().parent_path() / newName;
                    if (IsValidName(newName) && IsPathInProject(newPath)) {
                        fs::rename(entry.path(), newPath);
                    }
                    m_isRenaming = false;
                    renameDone = true;
                }
                if (ImGui::IsItemDeactivatedAfterEdit()) {
                    m_isRenaming = false;
                }
                ImGui::PopID();
            } else {
                ImGui::Button(("📁##" + name).c_str(), ImVec2(s_iconSize, s_iconSize));
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
                    m_selectedFolder = entry.path();
                }
                if (ImGui::IsItemClicked()) {
                    m_selectedFile = entry.path();
                }
                if (ImGui::BeginPopupContextItem()) {
                    ShowContextMenu(folder, entry.path(), true, true);
                    ImGui::EndPopup();
                }
                // Affiche sans extension en mode icône
                ImGui::TextWrapped("%s", name.c_str());
            }
            ImGui::EndGroup();
            ImGui::NextColumn();
        } else {
            // Mode liste
            ImGui::PushID(entry.path().string().c_str());

            // Renommage
            if (m_isRenaming && entry.path() == m_renamingPath) {
                ImGui::PushID(name.c_str());
                ImGui::SetKeyboardFocusHere();
                if (ImGui::InputText("##Rename", m_renamingBuffer, sizeof(m_renamingBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
                    std::string newName(m_renamingBuffer);
                    auto newPath = entry.path().parent_path() / newName;
                    if (IsValidName(newName) && IsPathInProject(newPath)) {
                        fs::rename(entry.path(), newPath);
                    }
                    m_isRenaming = false;
                    renameDone = true;
                }
                if (ImGui::IsItemDeactivatedAfterEdit()) {
                    m_isRenaming = false;
                }
                ImGui::PopID();
            } else {
                std::string label = name;
                if (ImGui::Selectable(label.c_str(), false)) {
                    m_selectedFolder = entry.path();
                }
                if (ImGui::BeginPopupContextItem()) {
                    ShowContextMenu(folder, entry.path(), true, true);
                    ImGui::EndPopup();
                }
            }
            ImGui::PopID();
        }
    }

    // Création de dossier (InputText hors boucle si le dossier n'existe pas encore)
    if (m_isRenaming && !fs::exists(m_renamingPath) && !m_renamingPath.empty()) {
        ImGui::PushID(m_renamingPath.string().c_str());
        ImGui::SetKeyboardFocusHere();
        if (ImGui::InputText("##RenameNewFolder", m_renamingBuffer, sizeof(m_renamingBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
            std::string newName(m_renamingBuffer);
            auto newPath = m_renamingPath.parent_path() / newName;
            if (IsValidName(newName) && IsPathInProject(newPath) && !fs::exists(newPath)) {
                fs::create_directory(newPath);
            }
            m_isRenaming = false;
            m_renamingPath.clear();
            m_selectedFolder = newPath;
        }
        if (ImGui::IsItemDeactivatedAfterEdit()) {
            m_isRenaming = false;
            m_renamingPath.clear();
        }
        ImGui::PopID();
    }

    // Puis fichiers
    for (const auto& entry : fs::directory_iterator(folder)) {
        if (entry.is_directory()) continue;
        std::string name = entry.path().filename().string();
        std::string nameNoExt = entry.path().stem().string();

        if (iconMode) {
            ImGui::BeginGroup();

            if (m_isRenaming && entry.path() == m_renamingPath) {
                ImGui::PushID("Rename");
                ImGui::SetKeyboardFocusHere();
                if (ImGui::InputText("##Rename", m_renamingBuffer, sizeof(m_renamingBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
                    std::string newName(m_renamingBuffer);
                    auto newPath = entry.path().parent_path() / newName;
                    if (IsValidName(newName) && IsPathInProject(newPath)) {
                        fs::rename(entry.path(), newPath);
                    }
                    m_isRenaming = false;
                    renameDone = true;
                }
                if (ImGui::IsItemDeactivatedAfterEdit()) {
                    m_isRenaming = false;
                }
                ImGui::PopID();
            }
            else {
                ImGui::Button(("📄##" + name).c_str(), ImVec2(s_iconSize, s_iconSize));

                // std::string iconName = GetIconNameForFile(entry.path());

                // IconManager::Instance().ShowIcon(iconName, ImVec2(s_iconSize, s_iconSize));

                if (ImGui::IsItemClicked()) {
                    HandleFileSelection(entry.path());
                }

                if (ImGui::BeginPopupContextItem()) {
                    ShowContextMenu(folder, entry.path(), false, true);
                    ImGui::EndPopup();
                }
                // Affiche sans extension en mode icône
                ImGui::TextWrapped("%s", nameNoExt.c_str());
            }

            ImGui::EndGroup();
            ImGui::NextColumn();
        } else {
            if (m_isRenaming && entry.path() == m_renamingPath) {
                ImGui::PushID(name.c_str());
                ImGui::SetKeyboardFocusHere();
                if (ImGui::InputText("##Rename", m_renamingBuffer, sizeof(m_renamingBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
                    std::string newName(m_renamingBuffer);
                    auto newPath = entry.path().parent_path() / newName;
                    if (IsValidName(newName) && IsPathInProject(newPath)) {
                        fs::rename(entry.path(), newPath);
                    }
                    m_isRenaming = false;
                    renameDone = true;
                }
                if (ImGui::IsItemDeactivatedAfterEdit()) {
                    m_isRenaming = false;
                }
                ImGui::PopID();
            } else {
                std::string label;
                std::string ext = entry.path().extension().string();
                if (ext == ".cpp" || ext == ".h") {
                    label = name; // avec extension
                } else {
                    label = nameNoExt; // sans extension
                }

                if (ImGui::Selectable(label.c_str(), false)) {
                    HandleFileSelection(entry.path());
                }

                if (ImGui::BeginPopupContextItem()) {
                    ShowContextMenu(folder, entry.path(), false, true);
                    ImGui::EndPopup();
                }
            }
        }
    }
    if (iconMode) ImGui::Columns(1);

    // Menu contextuel dans le vide (création uniquement)
    if (ImGui::BeginPopupContextWindow("RightPanelContext", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
        ShowContextMenu(folder, {}, false, false);
        ImGui::EndPopup();
    }

    // Création de matériau
    if (m_isCreatingMaterial) {
        ImGui::OpenPopup("Créer un matériau");
    }
    if (ImGui::BeginPopupModal("Créer un matériau", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::InputText("Nom du matériau", m_newMaterialName, sizeof(m_newMaterialName));
        if (ImGui::Button("Valider")) {
            std::string baseName = m_newMaterialName;
            std::string uniqueName = baseName;
            int index = 1;
            while (fs::exists(m_contextFolderPath / (uniqueName + ".mat"))) {
                uniqueName = baseName + "_" + std::to_string(index++);
            }
            auto matPath = m_contextFolderPath / (uniqueName + ".mat");
            if (IsValidName(uniqueName) && IsPathInProject(matPath)) {
                std::ofstream out(matPath);
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
            }
            m_isCreatingMaterial = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Annuler")) {
            m_isCreatingMaterial = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    // Création de script
    if (m_isCreatingScript) {
        ImGui::OpenPopup("Créer un script");
    }

    if (ImGui::BeginPopupModal("Créer un script", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::InputText("Nom du script", m_newScriptName, sizeof(m_newScriptName));
        if (ImGui::Button("Valider")) {
            std::string baseName = m_newScriptName;
            std::string uniqueName = baseName;
            int index = 1;

            fs::path headerPath, sourcePath;

            do {
                headerPath = m_contextFolderPath / (uniqueName + ".h");
                sourcePath = m_contextFolderPath / (uniqueName + ".cpp");
                if (fs::exists(headerPath) || fs::exists(sourcePath)) {
                    uniqueName = baseName + "_" + std::to_string(index++);
                } else break;
            } while (true);

            if (IsValidName(uniqueName) && IsPathInProject(headerPath) && IsPathInProject(sourcePath)) {
                std::ofstream outH(headerPath), outCpp(sourcePath);

                if (outH.is_open()) {
                    outH << "#pragma once\n\n"
                        << "// " << uniqueName << ".h\n\n"
                        << "class " << uniqueName << " {\n"
                        << "public:\n"
                        << "    " << uniqueName << "();\n"
                        << "    ~" << uniqueName << "();\n"
                        << "    void Update();\n"
                        << "};\n";
                    outH.close();
                }

                if (outCpp.is_open()) {
                    outCpp << "#include \"" << uniqueName << ".h\"\n\n"
                        << uniqueName << "::" << uniqueName << "() {}\n"
                        << uniqueName << "::~" << uniqueName << "() {}\n"
                        << "void " << uniqueName << "::Update() {}\n";
                    outCpp.close();
                }

                auto newScript = std::make_shared<ScriptComponent>();
                newScript->SetPathHeader(headerPath.string());
                newScript->SetPathSource(sourcePath.string());
                newScript->SetName(uniqueName);
                ScriptManager::Instance().AddScript(newScript);
            }

            m_isCreatingScript = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if (ImGui::Button("Annuler")) {
            m_isCreatingScript = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }


    // Préparation du renommage lors de la création de dossier
    if (m_isCreatingFolder) {
        m_isRenaming = true;
        m_renamingPath = m_contextFolderPath / m_renamingFolder;
        strncpy(m_renamingBuffer, m_renamingFolder.c_str(), sizeof(m_renamingBuffer));
        m_renamingBuffer[sizeof(m_renamingBuffer) - 1] = '\0';
        m_isCreatingFolder = false;
    }

    // Pour éviter bug de renommage si on supprime l'item en cours de renommage
    if (renameDone) {
        m_renamingPath.clear();
    }
}

void ProjectWindow::Render() {
    ImGui::Begin("Project");

    // Splitter horizontal
    ImGui::BeginChild("LeftPanel", ImVec2(s_leftPanelWidth, 0), true);
    RenderFolderTree(m_projectPath);
    ImGui::EndChild();

    ImGui::SameLine();

    // --- SPLITTER AMÉLIORÉ ---
    float splitterWidth = 6.0f;
    ImVec2 splitterMin = ImGui::GetCursorScreenPos();
    ImVec2 splitterMax = ImVec2(splitterMin.x + splitterWidth, splitterMin.y + ImGui::GetContentRegionAvail().y);

    ImGui::InvisibleButton("Splitter", ImVec2(splitterWidth, ImGui::GetContentRegionAvail().y));
    bool hovered = ImGui::IsItemHovered();
    bool active = ImGui::IsItemActive();

    // Couleur du splitter : plus clair si survolé ou drag
    ImU32 splitterColor = active ? IM_COL32(180,180,200,255) : hovered ? IM_COL32(140,140,160,255) : IM_COL32(100,100,120,255);
    ImGui::GetWindowDrawList()->AddRectFilled(splitterMin, splitterMax, splitterColor);

    if (active && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        float totalWidth = ImGui::GetWindowWidth();
        float mouseDelta = ImGui::GetIO().MouseDelta.x;
        float newLeftWidth = s_leftPanelWidth + mouseDelta;
        float maxLeftWidth = totalWidth - s_minRightPanelWidth - splitterWidth;
        if (newLeftWidth < s_minPanelWidth) newLeftWidth = s_minPanelWidth;
        if (newLeftWidth > maxLeftWidth) newLeftWidth = maxLeftWidth;
        s_leftPanelWidth = newLeftWidth;
    }
    ImGui::SameLine();

    ImGui::BeginChild("RightPanel", ImVec2(0, 0), true);
    RenderDirectory(m_selectedFolder.empty() ? m_projectPath : m_selectedFolder);
    ImGui::EndChild();

    ImGui::End();
}

void ProjectWindow::SetProjectPath(const std::string& path) {
    m_projectPath = path;
    m_selectedFolder = std::filesystem::path(path) / "Assets";
}

bool ProjectWindow::IsValidName(const std::string& name) const {
    if (name.empty() || name.size() > 64) return false;
    if (name.find('/') != std::string::npos || name.find('\\') != std::string::npos) return false;
    if (name.find("..") != std::string::npos) return false;
    if (name[0] == '.') return false;
    for (char c : name) {
        if (!std::isalnum(c) && c != '_' && c != '-') return false;
    }
    return true;
}

bool ProjectWindow::IsPathInProject(const std::filesystem::path& path) const {
    auto absProject = fs::absolute(m_projectPath);
    auto absPath = fs::absolute(path);
    return std::mismatch(absProject.begin(), absProject.end(), absPath.begin()).first == absProject.end();
}

void ProjectWindow::HandleFileSelection(const fs::path& path) {
    m_selectedFile = path;
    std::string ext = path.extension().string();
    std::string nameNoExt = path.stem().string();

    if (ext == ".mat") {
        auto mat = MaterialManager::Instance().GetMaterial(nameNoExt);
        if (mat) {
            m_inspector->SetSelectedMaterial(mat);
        }
    } else if (ext == ".cpp" || ext == ".h") {
        auto script = ScriptManager::Instance().GetScript(nameNoExt);
        if (script) {
            if (ext == ".h")
                script->SetPathHeader(path.string());
            else
                script->SetPathSource(path.string());
            m_inspector->SetSelectedScript(script);
        }
    }
}

std::string ProjectWindow::GetIconNameForFile(const fs::path& path) {
    if (path.extension() == ".cpp" || path.extension() == ".h") return "file_config";
    if (path.extension() == ".mat") return "material";
    return "file"; // Fichier générique
}
