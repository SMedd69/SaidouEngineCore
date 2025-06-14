#include <Engine/Renderer.h>
#include <glad/gl.h>

glm::mat4 Renderer::s_viewMatrix = glm::mat4(1.0f);
glm::mat4 Renderer::s_projectionMatrix = glm::mat4(1.0f);
std::shared_ptr<Shader> Renderer::s_defaultShader = nullptr;

void Renderer::Init() {
    // Charge ton shader basique (avec u_Model, u_View, u_Projection)
    s_defaultShader = std::make_shared<Shader>("assets/shaders/basic.vert", "assets/shaders/basic.frag");
}

void Renderer::SetViewProjection(const glm::mat4& view, const glm::mat4& projection) {
    s_viewMatrix = view;
    s_projectionMatrix = projection;
}

void Renderer::DrawMesh(std::shared_ptr<Mesh> mesh, const glm::mat4& model, const std::string& /*materialName*/) {
    if (!mesh || !s_defaultShader)
        return;

    s_defaultShader->Bind();
    s_defaultShader->SetMat4("u_Model", model);
    s_defaultShader->SetMat4("u_View", s_viewMatrix);
    s_defaultShader->SetMat4("u_Projection", s_projectionMatrix);

    mesh->Bind(); // bind VAO
    glDrawElements(GL_TRIANGLES, mesh->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
}
