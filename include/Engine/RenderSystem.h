#pragma once
#include <memory>
#include <Core/Scene.h>

class RenderSystem {
public:
    static void RenderScene(const Scene& scene, const glm::mat4& view, const glm::mat4& projection);
};
