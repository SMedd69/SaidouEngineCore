#include <Engine/Engine.h>
#include <Engine/Launcher.h>
#include <iostream>

int main()
{
    bool quit = false;
    std::string projectPath;
    std::string nameProject;

    while (!quit)
    {
        Launcher launcher;
        projectPath = launcher.Run();

        if (projectPath.empty())
        {
            break; // Quitter si aucun projet choisi
        }

        nameProject = launcher.GetNameProject();

        Engine engine(true, projectPath, nameProject);

        // Si l'utilisateur ne veut plus revenir au launcher, on quitte
        if (!engine.ShouldReturnToLauncher())
        {
            quit = true;
        }
    }

    return 0;
}
