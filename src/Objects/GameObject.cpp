#include "Objects/GameObject.h"
#include "Objects/Component.h"

GameObject::GameObject(const std::string& name) : name(name) {}

void GameObject::AddComponent(std::shared_ptr<Component> component) {
    component->owner = this;
    components.push_back(component);
}