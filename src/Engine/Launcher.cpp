#include <Engine/Launcher.h>
#include <Engine/DefaultImGuiLayout.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <filesystem>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <GLFW/glfw3.h>
#include <regex>

namespace fs = std::filesystem;

// Validation du nom de projet (pas de /, \, .., etc.)
static bool IsValidProjectName(const std::string& name) {
    if (name.empty() || name.size() > 64) return false;
    if (name.find('/') != std::string::npos || name.find('\\') != std::string::npos) return false;
    if (name.find("..") != std::string::npos) return false;
    if (name[0] == '.') return false;
    std::regex allowed("^[a-zA-Z0-9_-]+$");
    return std::regex_match(name, allowed);
}

std::string Launcher::Run()
{
    if (!glfwInit()) {
        std::cerr << "Erreur GLFW (Launcher)" << std::endl;
        return "";
    }

    GLFWwindow* window = glfwCreateWindow(600, 400, "SaidouEngine - Launcher", nullptr, nullptr);
    if (!window) return "";

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags &= ~ImGuiConfigFlags_DockingEnable; // Désactive le docking
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    char projectName[128] = "";
    char basePathBuffer[256] = "C:/SaidouEngineProjects";
    std::string selectedProject;
    bool projectReady = false;
    int selectedIndex = -1;
    std::string errorMsg;

    auto listProjects = [&]() -> std::vector<std::string> {
        std::vector<std::string> projects;
        fs::path basePath(basePathBuffer);
        if (fs::exists(basePath) && fs::is_directory(basePath)) {
            for (const auto& entry : fs::directory_iterator(basePath)) {
                if (fs::is_directory(entry) && fs::exists(entry.path() / "Project.config")) {
                    projects.push_back(entry.path().filename().string());
                }
            }
        }
        return projects;
    };

    std::vector<std::string> projects = listProjects();

    while (!glfwWindowShouldClose(window) && !projectReady)
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Centrage de la fenêtre principale
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Always);

        ImGui::Begin("SaidouEngine Launcher", nullptr,
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoTitleBar);

        ImGui::Columns(2, nullptr, true);

        // Colonne gauche : projets existants
        ImGui::Text("Projets existants");
        ImGui::Separator();
        ImGui::BeginChild("ProjectsList", ImVec2(0, -ImGui::GetFrameHeightWithSpacing() * 4), true);
        for (int i = 0; i < projects.size(); ++i) {
            bool isSelected = (i == selectedIndex);
            if (ImGui::Selectable(projects[i].c_str(), isSelected)) {
                selectedIndex = i;
                strcpy(projectName, projects[i].c_str());
                errorMsg.clear();
            }
        }
        ImGui::EndChild();

        if (selectedIndex >= 0) {
            if (ImGui::Button("Charger le projet sélectionné", ImVec2(-1, 0))) {
                selectedProject = std::string(basePathBuffer) + "/" + projects[selectedIndex];
                projectReady = true;
            }
        }

        ImGui::NextColumn();

        // Colonne droite : création de projet
        ImGui::Text("Créer un nouveau projet");
        ImGui::Separator();
        ImGui::InputText("Nom du projet", projectName, IM_ARRAYSIZE(projectName));
        ImGui::InputText("Répertoire de base", basePathBuffer, IM_ARRAYSIZE(basePathBuffer));
        if (ImGui::Button("Actualiser les projets existants")) {
            projects = listProjects();
            selectedIndex = -1;
            errorMsg.clear();
        }

        if (ImGui::Button("Créer le projet", ImVec2(-1, 0))) {
            std::string finalPath = std::string(basePathBuffer) + "/" + std::string(projectName);
            if (!IsValidProjectName(projectName)) {
                errorMsg = "Nom de projet invalide (lettres, chiffres, _ et - uniquement)";
            } else if (fs::exists(finalPath)) {
                errorMsg = "Un projet avec ce nom existe déjà !";
            } else {
                fs::create_directories(finalPath + "/Assets");
                fs::create_directories(finalPath + "/Scenes");
                std::string projectIni = finalPath + "/imgui_layout.ini";
                std::ofstream dst(projectIni, std::ios::binary);
                dst << DEFAULT_IMGUI_LAYOUT;
                dst.close();
                std::ofstream config(finalPath + "/Project.config");
                config << "project_name=" << projectName << std::endl;
                config.close();
                projectReady = true;
                selectedProject = finalPath;
            }
        }

        if (!errorMsg.empty()) {
            ImGui::Spacing();
            ImGui::TextColored(ImVec4(1,0.2f,0.2f,1), "%s", errorMsg.c_str());
        }

        ImGui::Columns(1);

        ImGui::End();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.15f, 0.15f, 0.18f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return selectedProject;
}

const std::string& Launcher::GetNameProject()
{
    return m_nameProjet;
}

void Launcher::SetNameProject(const std::string &nameProj)
{
    m_nameProjet = nameProj;
}