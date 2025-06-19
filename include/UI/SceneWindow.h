#pragma once

#include <UI/IUIWindow.h>
#include <Core/Scene.h>
#include <Engine/Shader.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>

class SceneWindow : public IUIWindow
{
    public:
        SceneWindow(GLFWwindow* window);
        ~SceneWindow();

        void Render() override;
        void ProcessInput();

        std::shared_ptr<Scene> GetScene() const { return m_scene; }
        std::shared_ptr<Shader> m_defaultShader;

        // Setters pour activer ou désactiver la Grid
        void SetGridEnabled(bool enabled) { m_gridEnabled = enabled; }
        bool IsGridEnabled() const { return m_gridEnabled; }

    private:
        GLFWwindow* m_window;

        std::shared_ptr<Scene> m_scene;

        bool m_gridEnabled = true; // Pour activer/désactiver la grille

        unsigned int m_framebuffer = 0;
        unsigned int m_renderTexture = 0;
        unsigned int m_depthBuffer = 0;

        // Camera parameters
        glm::vec3 m_target = glm::vec3(0.0f);
        float m_distance = 15.0f; // plus éloigné
        float m_pitch = glm::radians(45.0f); // angle plus plongeant
        float m_yaw = glm::radians(45.0f);
        float m_fov = 60.0f; // champ de vision plus large

        // Mouse tracking
        bool m_leftMousePressed = false;
        bool m_rightMousePressed = false;
        double m_lastX = 0.0, m_lastY = 0.0;

        glm::mat4 GetViewMatrix() const;
        glm::vec3 GetCameraPosition() const;
        glm::vec3 GetCameraDirection() const;

        void ProcessMouseMovement(float dx, float dy, bool orbiting, bool panning);
        void DrawGrid();
        void FocusCameraOnSelected();

        // OpenGL resources for the grid
        unsigned int m_gridVAO = 0;
        unsigned int m_gridVBO = 0;
        unsigned int m_gridVertexCount = 0;
        unsigned int m_gridShaderProgram = 0;
        unsigned int m_objectShaderProgram = 0;

        // Viewport size (pour passer aux shaders)
        int m_viewportWidth = 1;
        int m_viewportHeight = 1;
        int m_fbWidth;
        int m_fbHeight;

        // Helpers shaders
        unsigned int CompileShader(unsigned int type, const char* source);
        unsigned int CreateShaderProgram(const char* vertexSrc, const char* fragmentSrc);
        void UpdateFramebufferIfNeeded();

        void InitGrid();
};
