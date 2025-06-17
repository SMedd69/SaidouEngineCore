#pragma once
#include <memory>
#include <glm/glm.hpp>
#include <Engine/Mesh.h>
#include <Engine/Skybox.h>
#include <Core/Scene.h>
#include <Engine/Shader.h> // Un wrapper de shader que tu devras créer ou que tu as peut-être déjà
#include <string>

class Renderer {
public:
    static void Init(); // Chargement des shaders
    static void DrawMesh(std::shared_ptr<Mesh> mesh, const glm::mat4& model, const std::string& materialName);
    static void SetViewProjection(const glm::mat4& view, const glm::mat4& projection);
    static void SetSkybox(std::shared_ptr<Skybox> skybox);
    // Creaation de GetSkyboxFiles
    static std::vector<std::string> GetSkyboxFiles();
    static std::shared_ptr<Skybox> GetSkybox();
    static unsigned int RenderSceneToTexture(
        Scene* scene,
        const glm::mat4& view,
        const glm::mat4& projection,
        int width,
        int height);

    static void InitGameFramebuffer(int width, int height);
    // Renderer.h
    static void SetLightUniforms(const glm::vec3& dir, const glm::vec3& color, float intensity);

private:
    static glm::mat4 s_viewMatrix;
    static glm::mat4 s_projectionMatrix;
    static std::shared_ptr<Shader> s_defaultShader;
};
