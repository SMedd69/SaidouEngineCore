#include <UI/SceneWindow.h>
#include "Input/InputManager.h"
#include <imgui.h>
#include "UI/Tools/ImGuizmo.h"
#include <iostream>
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include <Objects/TransformComponent.h>
#include <Objects/DirectionnalLightComponent.h>
#include <Objects/MeshFilter.h>
#include <Objects/MeshRenderer.h>
#include <Engine/MaterialManager.h>
#include <Engine/RenderSystem.h>
#include <Engine/Renderer.h>

// --- Shaders pour la grille uniquement ---
const char* vertexShaderSrc = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
out vec3 vColor;
uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
void main() {
    vColor = aColor;
    gl_Position = u_Projection * u_View * u_Model * vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSrc = R"(
#version 330 core
in vec3 vColor;
out vec4 FragColor;
void main()
{
    FragColor = vec4(vColor, 1.0);
}
)";
struct GridVertex {
    glm::vec3 pos;
    glm::vec3 color;
};
SceneWindow::SceneWindow(GLFWwindow* window)
    : m_window(window), m_fbWidth(1), m_fbHeight(1)
{
    m_scene = std::make_shared<Scene>();
    glfwGetCursorPos(m_window, &m_lastX, &m_lastY);

    m_gridShaderProgram = CreateShaderProgram(vertexShaderSrc, fragmentShaderSrc);

    InitGrid();

    glGenFramebuffers(1, &m_framebuffer);
    glGenTextures(1, &m_renderTexture);
    glBindTexture(GL_TEXTURE_2D, m_renderTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenRenderbuffers(1, &m_depthBuffer);
}

void SceneWindow::FocusCameraOnSelected()
{
    if (!m_scene || !m_scene->selectedObject)
        return;

    auto transform = m_scene->selectedObject->GetComponent<TransformComponent>();
    if (!transform)
        return;

    // Centre la caméra sur la position de l'objet sélectionné
    m_target = transform->position;

    // Optionnel : ajuste la distance pour que l'objet soit bien visible
    m_distance = 8.0f; // ou adapte selon la taille de l'objet
}

SceneWindow::~SceneWindow()
{
    glDeleteProgram(m_gridShaderProgram);
    glDeleteBuffers(1, &m_gridVBO);
    glDeleteVertexArrays(1, &m_gridVAO);
    glDeleteFramebuffers(1, &m_framebuffer);
    glDeleteTextures(1, &m_renderTexture);
    glDeleteRenderbuffers(1, &m_depthBuffer);
}

unsigned int SceneWindow::CompileShader(unsigned int type, const char* source)
{
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Erreur compilation shader : " << infoLog << std::endl;
    }
    return shader;
}

unsigned int SceneWindow::CreateShaderProgram(const char* vertexSrc, const char* fragmentSrc)
{
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexSrc);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentSrc);

    unsigned int program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "Erreur linking shader program : " << infoLog << std::endl;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

void SceneWindow::InitGrid()
{
    // Initialisation du VAO/VBO pour la grille dynamique
    glGenVertexArrays(1, &m_gridVAO);
    glGenBuffers(1, &m_gridVBO);

    glBindVertexArray(m_gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_gridVBO);
    // On ne remplit pas ici, ce sera fait dynamiquement dans DrawGrid()
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GridVertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GridVertex), (void*)offsetof(GridVertex, color));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

glm::vec3 SceneWindow::GetCameraPosition() const
{
    return m_target + glm::vec3(
        m_distance * cos(m_pitch) * sin(m_yaw),
        m_distance * sin(m_pitch),
        m_distance * cos(m_pitch) * cos(m_yaw)
    );
}

glm::vec3 SceneWindow::GetCameraDirection() const
{
    return glm::normalize(m_target - GetCameraPosition());
}

glm::mat4 SceneWindow::GetViewMatrix() const
{
    return glm::lookAt(GetCameraPosition(), m_target, glm::vec3(0, 1, 0));
}

void SceneWindow::ProcessMouseMovement(float dx, float dy, bool orbiting, bool panning)
{
    if (orbiting) {
        float sensitivity = 0.005f;
        m_yaw   -= dx * sensitivity;
        m_pitch -= dy * sensitivity;

        float limit = glm::radians(89.0f);
        m_pitch = glm::clamp(m_pitch, -limit, limit);
    } else if (panning) {
        float panSpeed = 0.01f * m_distance;
        glm::vec3 right = glm::normalize(glm::cross(GetCameraDirection(), glm::vec3(0, 1, 0)));
        glm::vec3 up = glm::normalize(glm::cross(right, GetCameraDirection()));
        m_target += -right * dx * panSpeed + up * dy * panSpeed;
    }
}

void SceneWindow::ProcessInput()
{
    if (ImGuizmo::IsUsing() || ImGuizmo::IsOver())
        return;

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 scenePos = ImGui::GetCursorScreenPos();
    ImVec2 mousePos = io.MousePos;

    bool mouseOverScene =
        mousePos.x >= scenePos.x && mousePos.x < scenePos.x + m_viewportWidth &&
        mousePos.y >= scenePos.y && mousePos.y < scenePos.y + m_viewportHeight;

    if (mouseOverScene && ImGui::IsKeyPressed(ImGuiKey_F)) {
        FocusCameraOnSelected();
    }
    if (!mouseOverScene)
        return;

    const auto& events = InputManager::Instance().GetEvents();

    float dx = 0.0f;
    float dy = 0.0f;
    bool gotMouseMove = false;

    for (const auto& e : events)
    {
        if (e.type == InputEventType::MouseMove)
        {
            dx += static_cast<float>(e.mouseMove.x - m_lastX);
            dy += static_cast<float>(e.mouseMove.y - m_lastY);

            m_lastX = e.mouseMove.x;
            m_lastY = e.mouseMove.y;

            gotMouseMove = true;
        }
        else if (e.type == InputEventType::Scroll)
        {
            float zoomSpeed = 0.6f;
            m_distance -= float(e.scroll.yOffset) * zoomSpeed;
            m_distance = glm::clamp(m_distance, 1.0f, 100.0f);
        }
    }

    if (!gotMouseMove)
    {
        glfwGetCursorPos(m_window, &m_lastX, &m_lastY);
    }

    bool leftPressed = glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    bool rightPressed = glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

    if (leftPressed || rightPressed)
    {
        ProcessMouseMovement(dx, dy, leftPressed, rightPressed);
    }
}

void SceneWindow::DrawGrid()
{
    if (!m_gridEnabled)
    {
        std::cerr << "Grille désactivée, pas de rendu." << std::endl;
        return;
    }
    if (m_gridVAO == 0 || m_gridVBO == 0)
    {
        std::cerr << "Grille non initialisée, pas de rendu." << std::endl;
        return;
    }
    glUseProgram(m_gridShaderProgram);

    glm::vec3 camPos = GetCameraPosition();
    float camDist = glm::length(camPos - m_target);

    // --- Calcul du pas dynamique ---
    float baseStep = 1.0f;
    float logStep = std::pow(10.0f, std::floor(std::log10(camDist * 0.5f)));
    float step = baseStep * logStep;

    int halfLines = 100; // nombre de lignes de chaque côté (suffisant pour couvrir l'écran)

    std::vector<GridVertex> vertices;

    glm::vec3 gridColor(0.3f, 0.3f, 0.3f);
    glm::vec3 mainColor(0.6f, 0.6f, 0.6f);
    glm::vec3 axisXColor(1.0f, 0.2f, 0.2f);
    glm::vec3 axisZColor(0.2f, 0.4f, 1.0f);

    float gridY = 0.0f;

    int camX = int(std::round(camPos.x / step));
    int camZ = int(std::round(camPos.z / step));

    for (int i = -halfLines; i <= halfLines; ++i) {
        float x = (camX + i) * step;
        glm::vec3 color = (i == 0) ? axisXColor : (( (camX + i) % 10 == 0 ) ? mainColor : gridColor);
        vertices.push_back({ glm::vec3(x, gridY, (camZ - halfLines) * step), color });
        vertices.push_back({ glm::vec3(x, gridY, (camZ + halfLines) * step), color });

        float z = (camZ + i) * step;
        color = (i == 0) ? axisZColor : (( (camZ + i) % 10 == 0 ) ? mainColor : gridColor);
        vertices.push_back({ glm::vec3((camX - halfLines) * step, gridY, z), color });
        vertices.push_back({ glm::vec3((camX + halfLines) * step, gridY, z), color });
    }

    // Upload dynamique du buffer
    glBindVertexArray(m_gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_gridVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GridVertex), vertices.data(), GL_DYNAMIC_DRAW);

    // Matrices
    float aspect = m_viewportHeight > 0 ? float(m_viewportWidth) / m_viewportHeight : 1.0f;
    glm::mat4 projection = glm::perspective(glm::radians(m_fov), aspect, 0.1f, 100.0f);
    glm::mat4 view = GetViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);

    int locModel = glGetUniformLocation(m_gridShaderProgram, "u_Model");
    int locView = glGetUniformLocation(m_gridShaderProgram, "u_View");
    int locProj = glGetUniformLocation(m_gridShaderProgram, "u_Projection");

    glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(locView, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(locProj, 1, GL_FALSE, &projection[0][0]);

    glDisable(GL_DEPTH_TEST);
    glLineWidth(1.0f);
    glDrawArrays(GL_LINES, 0, vertices.size());
    glEnable(GL_DEPTH_TEST);

    glBindVertexArray(0);
    glUseProgram(0);
}

void SceneWindow::UpdateFramebufferIfNeeded()
{
    if (m_viewportWidth == m_fbWidth && m_viewportHeight == m_fbHeight)
        return;

    m_fbWidth = m_viewportWidth;
    m_fbHeight = m_viewportHeight;

    glBindTexture(GL_TEXTURE_2D, m_renderTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_fbWidth, m_fbHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_fbWidth, m_fbHeight);

    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_renderTexture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Erreur : Framebuffer incomplet !" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneWindow::Render()
{
    ImGuizmo::Enable(true);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    bool isSceneOpen = ImGui::Begin("Scene", nullptr,
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    if (isSceneOpen)
    {
        ImVec2 avail = ImGui::GetContentRegionAvail();
        m_viewportWidth = static_cast<int>(avail.x);
        m_viewportHeight = static_cast<int>(avail.y);

        ProcessInput();
        UpdateFramebufferIfNeeded();

        glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
        glViewport(0, 0, m_viewportWidth, m_viewportHeight);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Matrices de caméra
        float aspect = m_viewportHeight > 0 ? static_cast<float>(m_viewportWidth) / m_viewportHeight : 1.0f;
        glm::mat4 projection = glm::perspective(glm::radians(m_fov), aspect, 0.1f, 3000.0f);
        glm::mat4 view = GetViewMatrix();

        glm::vec3 sunDir = glm::vec3(-1, -1, -1); // Valeur par défaut
        for (const auto& go : m_scene->gameObjects) {
            auto light = go->GetComponent<DirectionalLightComponent>();
            if (light) {
                sunDir = light->direction;
                break;
            }
        }
        if (Renderer::GetSkybox())
            Renderer::GetSkybox()->Draw(view, projection, sunDir);

        // --- Rendu des objets via RenderSystem ---
        RenderSystem::RenderScene(*m_scene, view, projection);

        DrawGrid();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("WantCaptureMouse: %d", io.WantCaptureMouse);

        // Affichage de la texture rendue dans ImGui
        ImGui::Image((ImTextureID)(uintptr_t)m_renderTexture, avail, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::SetItemAllowOverlap();

        ImVec2 imagePos = ImGui::GetItemRectMin();
        ImVec2 imageSize = ImGui::GetItemRectSize();

        // Correction pour docking :
        ImGuiViewport* viewport = ImGui::GetWindowViewport();
        ImVec2 imageScreenPos = imagePos;
        if (viewport)
            imageScreenPos = ImVec2(imagePos.x + viewport->Pos.x, imagePos.y + viewport->Pos.y);

        // === Gizmo de ImGuizmo ===
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(imageScreenPos.x, imageScreenPos.y, imageSize.x, imageSize.y);

        if (m_scene && m_scene->selectedObject)
        {
            auto transform = m_scene->selectedObject->GetComponent<TransformComponent>();
            if (!transform) return;

            glm::mat4 model = transform->GetTransformMatrix();

            static ImGuizmo::OPERATION currentOperation = ImGuizmo::TRANSLATE;
            static ImGuizmo::MODE currentMode = ImGuizmo::WORLD;

            if (ImGui::IsKeyPressed(ImGuiKey_T)) currentOperation = ImGuizmo::TRANSLATE;
            if (ImGui::IsKeyPressed(ImGuiKey_R)) currentOperation = ImGuizmo::ROTATE;
            if (ImGui::IsKeyPressed(ImGuiKey_S)) currentOperation = ImGuizmo::SCALE;

            ImGuizmo::Manipulate(
                glm::value_ptr(view),
                glm::value_ptr(projection),
                currentOperation, currentMode,
                glm::value_ptr(model)
            );

            if (ImGuizmo::IsUsing())
            {
                if (m_scene->selectedObject)
                {
                    auto transform = m_scene->selectedObject->GetComponent<TransformComponent>();
                    if (transform)
                    {
                        // Transformer matrice mondiale modifiée en matrice locale
                        if (auto parent = m_scene->selectedObject->parent.lock())
                        {
                            auto parentTransform = parent->GetComponent<TransformComponent>();
                            if (parentTransform)
                            {
                                glm::mat4 parentWorld = parentTransform->GetWorldTransformMatrix();
                                glm::mat4 localMatrix = glm::inverse(parentWorld) * model;
                                transform->SetFromMatrix(localMatrix);
                            }
                            else
                            {
                                transform->SetFromMatrix(model);
                            }
                        }
                        else
                        {
                            transform->SetFromMatrix(model);
                        }
                    }
                }
            }
        }
    }
    ImGui::PopStyleVar();
    ImGui::End();
    InputManager::Instance().ClearEvents();
}