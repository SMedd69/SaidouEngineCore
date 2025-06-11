#include <UI/SceneWindow.h>
#include "Input/InputManager.h"
#include <imgui.h>
#include <iostream>
#include <vector>

// Shader sources
const char* vertexShaderSrc = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
uniform mat4 uViewProj;
void main()
{
    gl_Position = uViewProj * vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSrc = R"(
#version 330 core
out vec4 FragColor;
void main()
{
    FragColor = vec4(0.5, 0.5, 0.5, 1.0);
}
)";

SceneWindow::SceneWindow(GLFWwindow* window)
    : m_window(window), m_fbWidth(1280), m_fbHeight(720)
{
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
    // if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem |
    //                             ImGuiHoveredFlags_ChildWindows |
    //                             ImGuiHoveredFlags_RootWindow))
    //     return;

    std::cout << "Process Input ok" << std::endl;
    // Récupérer la position et le mouvement
    const auto& events = InputManager::Instance().GetEvents();

    double mouseX = m_lastX;
    double mouseY = m_lastY;

    float dx = 0.0f;
    float dy = 0.0f;

    for (const auto& e : events)
    {
        if (e.type == InputEventType::MouseMove)
        {
            dx = static_cast<float>(e.mouseMove.x - mouseX);
            dy = static_cast<float>(e.mouseMove.y - mouseY);

            mouseX = e.mouseMove.x;
            mouseY = e.mouseMove.y;
        }
        else if (e.type == InputEventType::Scroll)
        {
            OnScroll(0.0, e.scroll.yOffset);
        }
    }

    m_lastX = mouseX;
    m_lastY = mouseY;

    // Ici, interroger directement GLFW pour savoir si les boutons sont maintenus
    bool leftPressed = glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    bool rightPressed = glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

    if (leftPressed || rightPressed)
    {
        ProcessMouseMovement(dx, dy, leftPressed, rightPressed);
    }
}


void SceneWindow::OnScroll(double, double yoffset)
{
    m_pendingScrollY += yoffset;
}

void SceneWindow::DrawGrid()
{
    glUseProgram(m_gridShaderProgram);

    float aspect = m_viewportHeight > 0 ? float(m_viewportWidth) / m_viewportHeight : 1.0f;
    glm::mat4 projection = glm::perspective(glm::radians(m_fov), aspect, 0.1f, 100.0f);
    glm::mat4 view = GetViewMatrix();
    glm::mat4 viewProj = projection * view;

    int loc = glGetUniformLocation(m_gridShaderProgram, "uViewProj");
    glUniformMatrix4fv(loc, 1, GL_FALSE, &viewProj[0][0]);

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
    bool isSceneOpen = ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    if (isSceneOpen)
    {
        ImVec2 avail = ImGui::GetContentRegionAvail();
        m_viewportWidth = (int)avail.x;
        m_viewportHeight = (int)avail.y;

        UpdateFramebufferIfNeeded();

        glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
        glViewport(0, 0, m_viewportWidth, m_viewportHeight);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        DrawGrid();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        ImGui::Image((ImTextureID)(uintptr_t)m_renderTexture, avail, ImVec2(0, 1), ImVec2(1, 0));

        ProcessInput();

        ImGui::Text("Distance: %.2f | Yaw: %.2f | Pitch: %.2f", m_distance, glm::degrees(m_yaw), glm::degrees(m_pitch));

        if(std::abs(m_pendingScrollY) > 0.01)
        {
            if (ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows))
            {
                float zoomSpeed = 0.6f;
                m_distance -= float(m_pendingScrollY) * zoomSpeed;
                m_distance = glm::clamp(m_distance, 1.0f, 100.0f);
            }
            m_pendingScrollY = 0.0;
        }
    }

    ImGui::End();
}
