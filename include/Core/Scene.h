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

    // ✅ Suppression récursive d'un GameObject et ses enfants
    void RemoveGameObject(GameObject* obj) {
        if (!obj) return;

        // 1. 🔁 Supprimer récursivement tous les enfants
        for (auto& child : obj->children) {
            RemoveGameObject(child.get());
        }
        obj->children.clear();

        // 2. ❌ Supprimer le lien depuis le parent
        if (auto parent = obj->parent.lock()) {
            parent->children.erase(
                std::remove_if(parent->children.begin(), parent->children.end(),
                    [&](const std::shared_ptr<GameObject>& c) {
                        return c.get() == obj;
                    }),
                parent->children.end()
            );
        }

        // 3. 🧹 Supprimer le GameObject du vecteur principal
        gameObjects.erase(
            std::remove_if(gameObjects.begin(), gameObjects.end(),
                [&](const std::shared_ptr<GameObject>& o) {
                    return o.get() == obj;
                }),
            gameObjects.end()
        );
    }

};
