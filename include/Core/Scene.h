#pragma once
#include <vector>
#include <memory>
#include "Objects/GameObject.h"

class Scene {
public:
    std::vector<std::shared_ptr<GameObject>> gameObjects;
    std::shared_ptr<GameObject> selectedObject;

    void AddGameObject(const std::shared_ptr<GameObject>& obj) {
        gameObjects.push_back(obj);
    }
};