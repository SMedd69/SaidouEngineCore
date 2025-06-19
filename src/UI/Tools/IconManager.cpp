#include <UI/Tools/IconManager.h>
#include <UI/Tools/TextureLoader.h> // À adapter selon ta fonction de chargement de texture
#include <iostream>

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

ImTextureRef IconManager::GetIcon(const std::string& name) {
    auto it = m_icons.find(name);
    if (it != m_icons.end()) {
        std::cout << "Icon : " + name << std::endl;
        return it->second;
    }
    return (ImTextureID)0;
}

void IconManager::ShowIcon(const std::string& name, const ImVec2& size) {
    ImTextureRef texId = GetIcon(name);

    // Si texture toujours invalide, éviter le crash
    if (texId.GetTexID()) {
        ImGui::Image(texId.GetTexID(), size);
    } else {
        // Afficher un dummy invisible mais sans crasher
        ImGui::Dummy(size);
    }
}
