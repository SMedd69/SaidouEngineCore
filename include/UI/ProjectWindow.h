#pragma once
#include <UI/IUIWindow.h>
#include <UI/InspectorWindow.h>
#include <string>
#include <filesystem>

class ProjectWindow : public IUIWindow {
    public:
        void Render() override;
        void SetProjectPath(const std::string& path);
        InspectorWindow* m_inspector = nullptr;
        void SetInspector(InspectorWindow* inspector) { m_inspector = inspector; }

    private:
        bool m_isCreatingMaterial = false;
        char m_newMaterialName[128] = "NewMaterial";

        std::string m_projectPath;
        std::string m_renamingFolder;
        std::filesystem::path m_selectedFolder = "Assets";
        std::filesystem::path m_contextFolderPath;

        bool m_isCreatingFolder = false;
        void ShowMenu();
        void RenderDirectory(const std::filesystem::path& path);
        void RenderFolderTree(const std::filesystem::path& path);
};