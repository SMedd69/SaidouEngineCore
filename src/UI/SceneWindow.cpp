#include <UI/SceneWindow.h>
#include "Input/InputManager.h"
#include <imgui.h>
#include "UI/Tools/ImGuizmo.h"
#include <iostream>
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include <Objects/TransformComponent.h>
#include <Objects/MeshFilter.h>
#include <Objects/MeshRenderer.h>
#include <Engine/MaterialManager.h>

// Shader sources
const char* vertexShaderSrc = R"(
#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main() {
    gl_Position = u_Projection * u_View * u_Model * vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSrc = R"(
#version 330 core
out vec4 FragColor;

uniform vec4 u_Albedo;
uniform float u_Metallic;
uniform float u_Roughness;

void main()
{
    // Simplification : ici juste afficher la couleur albedo (tu pourras faire un shading plus complexe plus tard)
    FragColor = u_Albedo;
}
)";

SceneWindow::SceneWindow(GLFWwindow* window)
    : m_window(window), m_fbWidth(1), m_fbHeight(1)
{
    m_scene = std::make_shared<Scene>();
    glfwGetCursorPos(m_window, &m_lastX, &m_lastY);
    // m_defaultShader = std::make_shared<Shader>("assets/shaders/basic.vert", "assets/shaders/basic.frag");

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

void SceneWindow::InitGrid(int size, float step)
{
    std::vector<glm::vec3> vertices;
    for (int i = -size; i <= size; ++i) {
        vertices.emplace_back(i * step, 0.0f, -size * step);
        vertices.emplace_back(i * step, 0.0f, size * step);
        vertices.emplace_back(-size * step, 0.0f, i * step);
        vertices.emplace_back(size * step, 0.0f, i * step);
    }

    m_gridVertexCount = vertices.size();

    glGenVertexArrays(1, &m_gridVAO);
    glGenBuffers(1, &m_gridVBO);

    glBindVertexArray(m_gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_gridVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

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
    if (ImGuizmo::IsUsing())
    {
        // On ne traite pas l'input caméra pendant la manipulation ou le survol du gizmo
        return;
    }

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 scenePos = ImGui::GetCursorScreenPos();
    ImVec2 mousePos = io.MousePos;

    bool mouseOverScene =
        mousePos.x >= scenePos.x && mousePos.x < scenePos.x + m_viewportWidth &&
        mousePos.y >= scenePos.y && mousePos.y < scenePos.y + m_viewportHeight;

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
    glUseProgram(m_gridShaderProgram);

    float aspect = m_viewportHeight > 0 ? float(m_viewportWidth) / m_viewportHeight : 1.0f;
    glm::mat4 projection = glm::perspective(glm::radians(m_fov), aspect, 0.1f, 100.0f);
    glm::mat4 view = GetViewMatrix();
    glm::mat4 model = glm::mat4(1.0f); // matrice identité

    int locModel = glGetUniformLocation(m_gridShaderProgram, "u_Model");
    int locView = glGetUniformLocation(m_gridShaderProgram, "u_View");
    int locProj = glGetUniformLocation(m_gridShaderProgram, "u_Projection");

    glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(locView, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(locProj, 1, GL_FALSE, &projection[0][0]);

    glBindVertexArray(m_gridVAO);
    glDrawArrays(GL_LINES, 0, m_gridVertexCount);
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
    bool isSceneOpen = ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    if (isSceneOpen)
    {
        ImVec2 avail = ImGui::GetContentRegionAvail();
        m_viewportWidth = static_cast<int>(avail.x);
        m_viewportHeight = static_cast<int>(avail.y);

        ProcessInput();
        InputManager::Instance().ClearEvents();
        UpdateFramebufferIfNeeded();

        glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
        glViewport(0, 0, m_viewportWidth, m_viewportHeight);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        DrawGrid();

        // Matrices de caméra
        float aspect = m_viewportHeight > 0 ? static_cast<float>(m_viewportWidth) / m_viewportHeight : 1.0f;
        glm::mat4 projection = glm::perspective(glm::radians(m_fov), aspect, 0.1f, 100.0f);
        glm::mat4 view = GetViewMatrix();

        // Utilisation de ton shader de base
        glUseProgram(m_gridShaderProgram);

        int locModel = glGetUniformLocation(m_gridShaderProgram, "u_Model");
        int locView = glGetUniformLocation(m_gridShaderProgram, "u_View");
        int locProj = glGetUniformLocation(m_gridShaderProgram, "u_Projection");

        glUniformMatrix4fv(locView, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(locProj, 1, GL_FALSE, &projection[0][0]);

        for (auto& obj : m_scene->gameObjects)
        {
            auto meshRenderer = obj->GetComponent<MeshRenderer>();
            auto meshFilter = obj->GetComponent<MeshFilter>();
            auto transform = obj->GetComponent<TransformComponent>();

            if (meshRenderer && meshFilter && transform && meshFilter->mesh)
            {
                glm::mat4 model = transform->GetWorldTransformMatrix();
                std::cout << "[Render] model[3]: "
                        << model[3].x << ", " << model[3].y << ", " << model[3].z << std::endl;
                std::cout << "[Render] " << obj->name << " Position: "
                        << transform->position.x << ", "
                        << transform->position.y << ", "
                        << transform->position.z << std::endl;
                glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);

                glBindVertexArray(meshFilter->mesh->GetVAO());

                auto material = MaterialManager::Instance().GetMaterial(meshRenderer->material->name);
                if (!material) {
                    material = MaterialManager::Instance().GetMaterial("Default");
                }

                int locAlbedo = glGetUniformLocation(m_gridShaderProgram, "u_Albedo");
                int locMetallic = glGetUniformLocation(m_gridShaderProgram, "u_Metallic");
                int locRoughness = glGetUniformLocation(m_gridShaderProgram, "u_Roughness");

                glUniform4fv(locAlbedo, 1, glm::value_ptr(material->albedo));
                glUniform1f(locMetallic, material->metallic);
                glUniform1f(locRoughness, material->roughness);
                glDrawElements(GL_TRIANGLES, meshFilter->mesh->GetIndexCount(), GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);
            }
        }

        glUseProgram(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Affichage de la texture rendue dans ImGui
        ImGui::Image((ImTextureID)(uintptr_t)m_renderTexture, avail, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::SetItemAllowOverlap();

        // ========================
        // === Gizmo de ImGuizmo ===
        // ========================
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();

        ImVec2 imagePos = ImGui::GetItemRectMin();
        ImVec2 imageSize = ImGui::GetItemRectSize();
        ImGuizmo::SetRect(imagePos.x, imagePos.y, imageSize.x, imageSize.y);

        if (m_scene && m_scene->selectedObject)
        {
            auto transform = m_scene->selectedObject->GetComponent<TransformComponent>();
            if (!transform) return;

            glm::mat4 model = transform->GetWorldTransformMatrix();

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

            if (ImGuizmo::IsUsing() && ImGuizmo::IsOver())
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

    ImGui::End();
}