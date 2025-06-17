// DirectionalLightComponent.h
#pragma once
#include <Objects/Component.h>
#include <glm/vec3.hpp>

class DirectionalLightComponent : public Component {
public:
    glm::vec3 direction = glm::vec3(1.2f, 1.0f, 1.3f);
    glm::vec3 color = glm::vec3(1.0f);
    float intensity = 1.0f;

    std::string GetTypeName() const override;

    void OnInspectorGUI() override;
};
