#include <Engine/Renderer.h>
#include <Engine/RenderSystem.h>
#include <Engine/MaterialManager.h>
#include <Objects/DirectionnalLightComponent.h>
#include <glad/gl.h>

glm::mat4 Renderer::s_viewMatrix = glm::mat4(1.0f);
glm::mat4 Renderer::s_projectionMatrix = glm::mat4(1.0f);
std::shared_ptr<Shader> Renderer::s_defaultShader = nullptr;

static std::shared_ptr<Skybox> s_skybox = nullptr;

static unsigned int s_gameFBO = 0;
static unsigned int s_gameTexture = 0;
static unsigned int s_gameDepth = 0;

void Renderer::Init() {
    // Charge ton shader basique (avec u_Model, u_View, u_Projection)
    s_defaultShader = std::make_shared<Shader>("assets/shaders/basic.vert", "assets/shaders/basic.frag");
}

void Renderer::SetViewProjection(const glm::mat4& view, const glm::mat4& projection) {
    s_viewMatrix = view;
    s_projectionMatrix = projection;
}

void Renderer::SetSkybox(std::shared_ptr<Skybox> skybox) {
    s_skybox = skybox;
}
std::shared_ptr<Skybox> Renderer::GetSkybox() {
    return s_skybox;
}

void Renderer::SetLightUniforms(const glm::vec3& dir, const glm::vec3& color, float intensity)
{
    if (!s_defaultShader) return;
    s_defaultShader->Bind();
    s_defaultShader->SetVec3("u_LightDirection", dir);
    s_defaultShader->SetVec3("u_LightColor", color);
    s_defaultShader->SetFloat("u_LightIntensity", intensity);
}

void Renderer::DrawMesh(std::shared_ptr<Mesh> mesh, const glm::mat4& model, const std::string& materialName) {
    if (!mesh || !s_defaultShader)
        return;

    s_defaultShader->Bind();
    s_defaultShader->SetMat4("u_Model", model);
    s_defaultShader->SetMat4("u_View", s_viewMatrix);
    s_defaultShader->SetMat4("u_Projection", s_projectionMatrix);

    // Passe la position de la caméra
    glm::vec3 camPos = glm::vec3(glm::inverse(s_viewMatrix)[3]);
    s_defaultShader->SetVec3("u_CamPos", camPos);


    // Récupère le matériau
    auto material = MaterialManager::Instance().GetMaterial(materialName);
    if (!material) {
        material = MaterialManager::Instance().GetMaterial("Default");
    }
    s_defaultShader->SetFloat("u_Metallic", material->metallic);
    s_defaultShader->SetFloat("u_Roughness", material->roughness);
    s_defaultShader->SetVec4("u_Albedo", material->albedo);

    // Ajoute ici la gestion de la lumière si besoin

    mesh->Bind();
    glDrawElements(GL_TRIANGLES, mesh->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
}

void Renderer::InitGameFramebuffer(int width, int height)
{
    if (s_gameFBO) {
        glDeleteFramebuffers(1, &s_gameFBO);
        glDeleteTextures(1, &s_gameTexture);
        glDeleteRenderbuffers(1, &s_gameDepth);
    }

    glGenFramebuffers(1, &s_gameFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, s_gameFBO);

    glGenTextures(1, &s_gameTexture);
    glBindTexture(GL_TEXTURE_2D, s_gameTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, s_gameTexture, 0);

    glGenRenderbuffers(1, &s_gameDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, s_gameDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, s_gameDepth);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int Renderer::RenderSceneToTexture(
    Scene* scene,
    const glm::mat4& view,
    const glm::mat4& projection,
    int width,
    int height)
{
    if (!s_gameFBO || !s_gameTexture)
        InitGameFramebuffer(width, height);

    glBindFramebuffer(GL_FRAMEBUFFER, s_gameFBO);
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::vec3 sunDir = glm::vec3(-1, -1, -1); // Valeur par défaut
    for (const auto& go : scene->gameObjects) {
        auto light = go->GetComponent<DirectionalLightComponent>();
        if (light) {
            sunDir = light->direction;
            break;
        }
    }
    if (Renderer::GetSkybox())
        Renderer::GetSkybox()->Draw(view, projection, sunDir);
    RenderSystem::RenderScene(*scene, view, projection);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return s_gameTexture;
}