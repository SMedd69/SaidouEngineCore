#pragma once
#include <UI/IUIWindow.h>
#include <UI/InspectorWindow.h>
#include <string>
#include <filesystem>
#include <algorithm>
#include <set>

namespace fs = std::filesystem;

class ProjectWindow : public IUIWindow {
public:
    void Render() override;
    void SetProjectPath(const std::string& path);
    InspectorWindow* m_inspector = nullptr;
    void SetInspector(InspectorWindow* inspector) { m_inspector = inspector; }
    void HandleFileSelection(const fs::path& path);
    std::string GetIconNameForFile(const fs::path& path);

private:
    bool m_isCreatingMaterial = false;
    char m_newMaterialName[128] = "NewMaterial";
    bool m_isCreatingScript = false;
    char m_newScriptName[128] = "NewScript";

    std::string m_projectPath;
    std::string m_renamingFolder;
    std::filesystem::path m_selectedFolder = "Assets";
    std::filesystem::path m_contextFolderPath;
    std::filesystem::path m_selectedFile;

    std::set<std::filesystem::path> m_selectedItems;

    bool m_isCreatingFolder = false;

    // Pour le renommage
    bool m_isRenaming = false;
    std::filesystem::path m_renamingPath;
    char m_renamingBuffer[256] = {};

    void RenderDirectory(const std::filesystem::path& path);
    void RenderFolderTree(const std::filesystem::path& path);
    void ShowContextMenu(const fs::path& targetFolder, const fs::path& targetItem, bool isFolder, bool isOnItem);

    // ...déjà présent...
    bool IsValidName(const std::string& name) const;
    bool IsPathInProject(const std::filesystem::path& path) const;
};