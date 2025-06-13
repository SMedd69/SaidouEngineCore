#pragma once
#include <vector>
#include <memory>
#include "Objects/GameObject.h"
#include <algorithm>

class Scene {
public:
    std::vector<std::shared_ptr<GameObject>> gameObjects;
    std::shared_ptr<GameObject> selectedObject;

    void AddGameObject(const std::shared_ptr<GameObject>& obj) {
        gameObjects.push_back(obj);
    }

    void Scene::RemoveGameObject(GameObject* obj) {
        gameObjects.erase(std::remove_if(gameObjects.begin(), gameObjects.end(),
            [&](const std::shared_ptr<GameObject>& o) {
                return o.get() == obj;
            }),
            gameObjects.end());
    }
};