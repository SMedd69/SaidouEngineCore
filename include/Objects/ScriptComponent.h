#pragma once
#include <Objects/Component.h>
#include <imgui.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>

class ScriptComponent : public Component {
public:
    enum class SelectedFile {
        Header,
        Source
    };

    SelectedFile selectedFile = SelectedFile::Header;

    bool started = false;

    std::string name = "NewScript";

    // Chemins vers les fichiers source et header
    std::string headerPath;
    std::string sourcePath;

    // Stocker le contenu lu (pour éviter de relire à chaque frame)
    std::string headerContent;
    std::string sourceContent;

    // Méthode pour charger le contenu des fichiers
    void LoadScriptFiles() {
        headerContent.clear();
        sourceContent.clear();

        // Lire header
        if (!headerPath.empty()) {
            std::ifstream file(headerPath);
            if (file.is_open()) {
                std::stringstream buffer;
                buffer << file.rdbuf();
                headerContent = buffer.str();
                file.close();
            }
        }

        // Lire source
        if (!sourcePath.empty()) {
            std::ifstream file(sourcePath);
            if (file.is_open()) {
                std::stringstream buffer;
                buffer << file.rdbuf();
                sourceContent = buffer.str();
                file.close();
            }
        }
    }

    void SetName(const std::string& n) { name = n; }
    std::string GetName() const { return name; }

    void SetPathHeader(const std::string& header) {
        headerPath = header;
        selectedFile = SelectedFile::Header;
        LoadHeaderFile();
    }

    void SetPathSource(const std::string& source) {
        sourcePath = source;
        selectedFile = SelectedFile::Source;
        LoadSourceFile();
    }

    void LoadHeaderFile() {
        headerContent.clear();
        if (!headerPath.empty()) {
            std::ifstream file(headerPath);
            if (file.is_open()) {
                std::stringstream buffer;
                buffer << file.rdbuf();
                headerContent = buffer.str();
            }
        }
    }

    void LoadSourceFile() {
        sourceContent.clear();
        if (!sourcePath.empty()) {
            std::ifstream file(sourcePath);
            if (file.is_open()) {
                std::stringstream buffer;
                buffer << file.rdbuf();
                sourceContent = buffer.str();
            }
        }
    }

    std::string GetTypeName() const override { return "Script"; }

    void OnInspectorGUI() override {
        ImGui::Text("Script: %s", name.c_str());
        ImGui::Separator();

        if (selectedFile == SelectedFile::Header) {
            ImGui::Text("Header (%s):", headerPath.c_str());
            ImGui::BeginChild("HeaderContent", ImVec2(0, 150), true, ImGuiWindowFlags_HorizontalScrollbar);
            ImGui::InputTextMultiline("##header", (char*)headerContent.data(), headerContent.size() + 1, ImVec2(-1, 0), ImGuiInputTextFlags_ReadOnly);
            ImGui::EndChild();
        }
        else if (selectedFile == SelectedFile::Source) {
            ImGui::Text("Source (%s):", sourcePath.c_str());
            ImGui::BeginChild("SourceContent", ImVec2(0, 150), true, ImGuiWindowFlags_HorizontalScrollbar);
            ImGui::InputTextMultiline("##source", (char*)sourceContent.data(), sourceContent.size() + 1, ImVec2(-1, 0), ImGuiInputTextFlags_ReadOnly);
            ImGui::EndChild();
        }
    }
};
