#pragma once

#include <string>
#include <vector>
#include <memory>
#include <atomic>
#include <glm/glm.hpp>

class Component;

class GameObject : public std::enable_shared_from_this<GameObject> {
public:
    std::string name;
    glm::vec3 position = {};
    glm::vec3 rotation = {};
    glm::vec3 scale = {1, 1, 1};

    std::vector<std::shared_ptr<Component>> components;
    unsigned int id;

    // 🎯 Hiérarchie
    std::weak_ptr<GameObject> parent;
    std::vector<std::shared_ptr<GameObject>> children;

    GameObject(const std::string& name);
    void AddComponent(std::shared_ptr<Component> component);

    void SetParent(std::shared_ptr<GameObject> newParent);

    template<typename T>
    T* GetComponent();

private:
    static std::atomic_uint s_nextId;
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
