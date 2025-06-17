#pragma once
#include <string>
#include <unordered_map>
#include <imgui.h>

class IconManager {
public:
    static IconManager& Instance();

    // Charge toutes les icônes à l'initialisation
    void Init();

    // Affiche l'icône par nom (ex: "folder_empty", "material", etc.)
    void ShowIcon(const std::string& name, ImVec2 size);

    // Récupère l'ImTextureID d'une icône
    ImTextureID GetIcon(const std::string& name);

private:
    std::unordered_map<std::string, ImTextureID> m_icons;
    IconManager() = default;
};