#include <UI/Tools/IconManager.h>
#include <UI/Tools/TextureLoader.h> // À adapter selon ta fonction de chargement de texture

IconManager& IconManager::Instance() {
    static IconManager instance;
    return instance;
}

void IconManager::Init() {
    // Charge toutes les icônes nécessaires
    m_icons["folder_empty"] = LoadTexture("assets/icons/folder_empty.png");
    m_icons["folder_full"]  = LoadTexture("assets/icons/folder_full.png");
    m_icons["material"]     = LoadTexture("assets/icons/material.png");
    m_icons["file_config"]         = LoadTexture("assets/icons/file-config.png");
    m_icons["file"]         = LoadTexture("assets/icons/file.png");
    // Ajoute d'autres icônes ici...
}

ImTextureID IconManager::GetIcon(const std::string& name) {
    auto it = m_icons.find(name);
    if (it != m_icons.end()) return it->second;
    return (ImTextureID)nullptr;
}

void IconManager::ShowIcon(const std::string& name, ImVec2 size) {
    ImTextureID tex = GetIcon(name);
    if (tex)
        ImGui::Image(tex, size);
    else
        ImGui::Dummy(size); // Affiche un espace vide si l'icône n'existe pas
}