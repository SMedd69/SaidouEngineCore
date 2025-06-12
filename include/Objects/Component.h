#pragma once
#include <string>

class GameObject;

class Component {
public:
    GameObject* owner;
    virtual std::string GetTypeName() const = 0;
    virtual void OnInspectorGUI() {}
    virtual ~Component() = default;
};
