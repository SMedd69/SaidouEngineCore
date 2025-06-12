#include "Engine/Engine.h"
#include "Input/InputManager.h"
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <iostream>

Engine::Engine(bool running, const std::string& projectPath, const std::string& projectName)
    : m_running(running), m_window(nullptr), m_projectPath(projectPath), m_projectName(projectName), m_showLauncher(false)
{
    Init();
    InitUI();
    Run();
    Shutdown();
}

void Engine::Init()
{
    // Init GLFW
    if (!glfwInit()) {
        std::cerr << "Erreur lors de l'initialisation de GLFW\n";
        exit(EXIT_FAILURE);
    }

    // Créer la fenêtre
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    std::string nameProject = "SaidouEngine | " + m_projectName;
    m_window = glfwCreateWindow(1280, 720, nameProject.c_str(), nullptr, nullptr);
    if (!m_window) {
        std::cerr << "Échec de la création de la fenêtre GLFW\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1); // VSync

    // Init GLAD
    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cerr << "Erreur lors de l'initialisation de GLAD\n";
        exit(EXIT_FAILURE);
    }

    // Init ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(m_window, false);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void Engine::InitUI() {
    m_dockspace = std::make_unique<DockspaceWindow>();
    m_scene = std::make_unique<SceneWindow>(m_window);
    m_hierarchy = std::make_unique<HierarchyWindow>();
    m_inspector = std::make_unique<InspectorWindow>();
    m_game = std::make_unique<GameWindow>();
    m_project = std::make_unique<ProjectWindow>();
    m_menuBar = std::make_unique<MenuBarUI>();
    m_menuBar->p_showLauncher = &m_showLauncher;

    m_hierarchy->SetScene(m_scene->GetScene().get());
    m_inspector->SetScene(m_scene->GetScene().get());

    InputManager::Instance().Init(m_window);
}

void Engine::Shutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Engine::ProcessEvents()
{
    if (glfwWindowShouldClose(m_window)) {
        m_running = false;
    }

    glfwPollEvents();
}

void Engine::BeginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Engine::RenderUI()
{
    if(m_dockspace) m_dockspace->Render();
    if(m_scene) m_scene->Render();
    if(m_hierarchy) m_hierarchy->Render();
    if(m_inspector) m_inspector->Render();
    if(m_game) m_game->Render();
    if(m_project) m_project->Render();
    if(m_menuBar) m_menuBar->Render();
}

void Engine::EndFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(m_window);
}

void Engine::Run()
{
    while (m_running)
    {
        if (m_showLauncher) {
            m_running = false; // Demande de retour au launcher -> quitte la boucle
            break;
        }

        ProcessEvents();
        InputManager::Instance().Update();

        BeginFrame();
        RenderUI();
        EndFrame();

        InputManager::Instance().ClearEvents();
    }
}
