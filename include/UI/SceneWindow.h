#pragma once

#include <UI/IUIWindow.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <vector>

class SceneWindow : public IUIWindow
{
public:
    SceneWindow(GLFWwindow* window);
    ~SceneWindow();

    void Render() override;

    void OnScroll(double xoffset, double yoffset);
    void ProcessInput();

private:
    GLFWwindow* m_window;

    unsigned int m_framebuffer = 0;
    unsigned int m_renderTexture = 0;
    unsigned int m_depthBuffer = 0;

    // Camera parameters
    glm::vec3 m_target = glm::vec3(0.0f);
    float m_distance = 10.0f;
    float m_pitch = glm::radians(30.0f);
    float m_yaw = glm::radians(45.0f);
    float m_fov = 45.0f;

    // Mouse tracking
    bool m_leftMousePressed = false;
    bool m_rightMousePressed = false;
    double m_lastX = 0.0, m_lastY = 0.0;
    double m_pendingScrollY = 0.0;

    glm::mat4 GetViewMatrix() const;
    glm::vec3 GetCameraPosition() const;
    glm::vec3 GetCameraDirection() const;

    void ProcessMouseMovement(float dx, float dy, bool orbiting, bool panning);
    void DrawGrid();

    // OpenGL resources for the grid
    unsigned int m_gridVAO = 0;
    unsigned int m_gridVBO = 0;
    unsigned int m_gridVertexCount = 0;
    unsigned int m_gridShaderProgram = 0;

    // Viewport size (pour passer aux shaders)
    int m_viewportWidth = 1;
    int m_viewportHeight = 1;
    int m_fbWidth;
    int m_fbHeight;

    // Helpers shaders
    unsigned int CompileShader(unsigned int type, const char* source);
    unsigned int CreateShaderProgram(const char* vertexSrc, const char* fragmentSrc);
    void UpdateFramebufferIfNeeded();

    void InitGrid(int size = 20, float step = 1.0f);
};
