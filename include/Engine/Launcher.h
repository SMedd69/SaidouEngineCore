#pragma once

#include <string>

class Launcher
{
    public:
        std::string Run(); // Affiche la fenêtre, retourne le chemin du projet sélectionné/créé
        const std::string& GetNameProject();
        void SetNameProject(const std::string& nameProj);

        std::string m_nameProjet;
};
