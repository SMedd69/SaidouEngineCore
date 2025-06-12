#pragma once

#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>

class Component;

class GameObject {
public:
    std::string name;
    glm::vec3 position = {};
    glm::vec3 rotation = {};
    glm::vec3 scale = {1, 1, 1};

    std::vector<std::shared_ptr<Component>> components;

    GameObject(const std::string& name);
    void AddComponent(std::shared_ptr<Component> component);

    template<typename T>
    T* GetComponent(); // ✅ Déclaration ici
};

// ✅ Implémentation en dehors de la classe mais toujours dans le header
template<typename T>
T* GameObject::GetComponent() {
    for (auto& comp : components) {
        if (auto ptr = dynamic_cast<T*>(comp.get())) {
            return ptr;
        }
    }
    return nullptr;
}
