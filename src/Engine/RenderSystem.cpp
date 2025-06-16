#include <Engine/RenderSystem.h>
#include <Objects/GameObject.h>
#include <Objects/MeshFilter.h>
#include <Objects/MeshRenderer.h>
#include <Objects/TransformComponent.h>
#include <Objects/DirectionnalLightComponent.h>
#include <Engine/Renderer.h> // Hypothétique classe qui a une méthode DrawMesh()

void RenderSystem::RenderScene(const Scene& scene, const glm::mat4& view, const glm::mat4& projection)
{
    // Recherche la première DirectionalLight de la scène
    glm::vec3 lightDir = glm::vec3(-1, -1, -1);
    glm::vec3 lightColor = glm::vec3(1, 1, 1);
    float lightIntensity = 1.0f;

    for (const auto& go : scene.gameObjects)
    {
        auto light = go->GetComponent<DirectionalLightComponent>();
        if (light)
        {
            lightDir = light->direction;
            lightColor = light->color;
            lightIntensity = light->intensity;
            break;
        }
    }

    Renderer::SetLightUniforms(lightDir, lightColor, lightIntensity);

    for (const auto& go : scene.gameObjects)
    {
        auto meshFilter = go->GetComponent<MeshFilter>();
        auto meshRenderer = go->GetComponent<MeshRenderer>();
        auto transform = go->GetComponent<TransformComponent>();

        if (meshFilter && meshRenderer && transform && meshFilter->mesh)
        {
            Renderer::SetViewProjection(view, projection);
            Renderer::DrawMesh(meshFilter->mesh, transform->GetWorldTransformMatrix(), meshRenderer->material->name);
        }
    }
}