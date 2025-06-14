#include <Engine/RenderSystem.h>
#include <Objects/GameObject.h>
#include <Objects/MeshFilter.h>
#include <Objects/MeshRenderer.h>
#include <Objects/TransformComponent.h>
#include <Engine/Renderer.h> // Hypothétique classe qui a une méthode DrawMesh()

void RenderSystem::RenderScene(const Scene& scene)
{
    for (const auto& go : scene.gameObjects)
    {
        auto meshFilter = go->GetComponent<MeshFilter>();
        auto meshRenderer = go->GetComponent<MeshRenderer>();
        auto transform = go->GetComponent<TransformComponent>();

        if (meshFilter && meshRenderer && transform && meshFilter->mesh)
        {
            // Hypothétique renderer global — peut être un singleton ou passé en paramètre
            Renderer::DrawMesh(meshFilter->mesh, transform->GetWorldTransformMatrix(), meshRenderer->material->name);
        }
    }
}
