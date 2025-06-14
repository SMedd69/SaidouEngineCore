#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <memory>
#include <string>
#include <UI/DockspaceWindow.h>
#include <UI/HierarchyWindow.h>
#include <UI/SceneWindow.h>
#include <UI/GameWindow.h>
#include <UI/InspectorWindow.h>
#include <UI/ProjectWindow.h>
#include <UI/Tools/MenuBarUI.h>

class Engine
{
    private:
        bool m_running;
        GLFWwindow* m_window;

        void Init();
        void InitUI();
        void InitializeMaterials();
        void Shutdown();
        void ProcessEvents();
        void BeginFrame();
        void RenderUI();
        void EndFrame();

    public:
        Engine(bool running, const std::string& projectPath, const std::string& nameProject);
        void Run();

        std::string m_projectPath;
        std::string m_projectName;
        bool m_showLauncher;

        bool ShouldReturnToLauncher() const { return m_showLauncher; }

        std::unique_ptr<DockspaceWindow> m_dockspace;
        std::unique_ptr<HierarchyWindow> m_hierarchy;
        std::unique_ptr<SceneWindow> m_scene;
        std::unique_ptr<GameWindow> m_game;
        std::unique_ptr<InspectorWindow> m_inspector;
        std::unique_ptr<ProjectWindow> m_project;
        std::unique_ptr<MenuBarUI> m_menuBar;
};