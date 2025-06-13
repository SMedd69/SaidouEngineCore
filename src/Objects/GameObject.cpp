#include "Objects/GameObject.h"
#include "Objects/Component.h"
#include <algorithm>

std::atomic_uint GameObject::s_nextId{1};

GameObject::GameObject(const std::string& name) : name(name) {
    id = s_nextId++;
}

void GameObject::AddComponent(std::shared_ptr<Component> component) {
    component->owner = this;
    components.push_back(component);
}

void GameObject::SetParent(std::shared_ptr<GameObject> newParent) {
    // Retirer de l'ancien parent s'il y en avait un
    if (auto currentParent = parent.lock()) {
        auto& siblings = currentParent->children;
        auto it = std::remove_if(siblings.begin(), siblings.end(),
    [this](const std::shared_ptr<GameObject>& child) {
        return child->id == this->id;
    });
siblings.erase(it, siblings.end());
    }

    parent = newParent;

    if (newParent) {
        newParent->children.push_back(shared_from_this());
    }
}