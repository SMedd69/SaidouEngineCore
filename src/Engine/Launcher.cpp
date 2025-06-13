#include <Engine/Launcher.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <filesystem>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <GLFW/glfw3.h>

namespace fs = std::filesystem;

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
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    char projectName[128] = "";
    char basePathBuffer[256] = "C:/SaidouEngineProjects";
    std::string selectedProject;
    bool projectReady = false;
    int selectedIndex = -1;

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

        ImGui::Begin("Créer ou charger un projet");

        ImGui::InputText("Répertoire de base", basePathBuffer, IM_ARRAYSIZE(basePathBuffer));
        if (ImGui::Button("Actualiser les projets existants")) {
            projects = listProjects();
            selectedIndex = -1;
        }

        ImGui::Separator();

        ImGui::Text("Créer un nouveau projet");
        ImGui::InputText("Nom du projet", projectName, IM_ARRAYSIZE(projectName));
        if (ImGui::Button("Créer le projet") && projectName[0] != '\0') {
            std::string finalPath = std::string(basePathBuffer) + "/" + std::string(projectName);
            fs::create_directories(finalPath + "/Assets");
            fs::create_directories(finalPath + "/Scenes");
            std::ofstream config(finalPath + "/Project.config");
            config << "project_name=" << projectName << std::endl;
            config.close();
            projectReady = true;
            selectedProject = finalPath;
            SetNameProject(projectName);
        }

        ImGui::Separator();

        ImGui::Text("Projets existants");
        for (int i = 0; i < projects.size(); ++i) {
            bool isSelected = (i == selectedIndex);
            if (ImGui::Selectable(projects[i].c_str(), isSelected)) {
                selectedIndex = i;
            }
        }

        if (selectedIndex >= 0) {
            if (ImGui::Button("Charger le projet sélectionné")) {
                selectedProject = std::string(basePathBuffer) + "/" + projects[selectedIndex];

                SetNameProject(projectName);
                projectReady = true;
            }
        }

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
