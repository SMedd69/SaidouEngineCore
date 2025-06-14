#pragma once
#include <memory>
#include <glm/glm.hpp>
#include <Engine/Mesh.h>
#include <Engine/Shader.h> // Un wrapper de shader que tu devras créer ou que tu as peut-être déjà
#include <string>

class Renderer {
public:
    static void Init(); // Chargement des shaders
    static void DrawMesh(std::shared_ptr<Mesh> mesh, const glm::mat4& model, const std::string& materialName);
    static void SetViewProjection(const glm::mat4& view, const glm::mat4& projection);

private:
    static glm::mat4 s_viewMatrix;
    static glm::mat4 s_projectionMatrix;
    static std::shared_ptr<Shader> s_defaultShader;
};
