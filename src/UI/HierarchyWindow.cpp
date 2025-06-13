#include <UI/HierarchyWindow.h>
#include <Objects/TransformComponent.h>
#include <imgui.h>
#include <iostream>

void HierarchyWindow::Render()
{
    if (m_scene)
    {
        DrawHierarchy(*m_scene);
    } else {
        std::cout << "Pas de scene\n";
    }
}

void HierarchyWindow::DrawHierarchy(Scene& scene) {
    ImGui::Begin("Hierarchy");

    bool clickedOnItem = false;
    std::shared_ptr<GameObject> objectToDelete = nullptr;

    // 💡 Afficher seulement les racines (pas d'objet parent)
    for (auto& obj : scene.gameObjects) {
        if (obj->parent.expired()) {
            DrawGameObjectNode(scene, obj, clickedOnItem, objectToDelete);
        }
    }

    // Déselection si clic gauche dans vide
    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0) && !clickedOnItem) {
        scene.selectedObject = nullptr;
    }

    // Menu contextuel global (dans vide uniquement si rien sélectionné)
    if (!scene.selectedObject) {
        if (ImGui::BeginPopupContextWindow("HierarchyContext", ImGuiPopupFlags_MouseButtonRight)) {
            if (ImGui::MenuItem("Créer un GameObject vide")) {
                auto go = std::make_shared<GameObject>("New GameObject");
                go->AddComponent(std::make_shared<TransformComponent>());
                scene.AddGameObject(go);
            }
            ImGui::EndPopup();
        }
    } else {
        // Menu contextuel avec un objet sélectionné
        if (ImGui::BeginPopupContextWindow("HierarchyContext", ImGuiPopupFlags_MouseButtonRight)) {
            if (ImGui::MenuItem("Créer un GameObject vide (enfant)")) {
                auto child = std::make_shared<GameObject>("New Child");
                child->AddComponent(std::make_shared<TransformComponent>());
                scene.AddGameObject(child);
                child->SetParent(scene.selectedObject);
            }
            ImGui::EndPopup();
        }
    }

    if (objectToDelete) {
        if (scene.selectedObject == objectToDelete)
            scene.selectedObject = nullptr;

        scene.RemoveGameObject(objectToDelete.get());
    }

    ImGui::End();
}

void HierarchyWindow::DrawGameObjectNode(Scene& scene, std::shared_ptr<GameObject> obj,
                                            bool& clickedOnItem, std::shared_ptr<GameObject>& objectToDelete) {
    ImGui::PushID(obj->id);

    ImGuiTreeNodeFlags flags = (scene.selectedObject == obj ? ImGuiTreeNodeFlags_Selected : 0) |
                            (obj->children.empty() ? ImGuiTreeNodeFlags_Leaf : 0) |
                            ImGuiTreeNodeFlags_OpenOnArrow;

    bool open = ImGui::TreeNodeEx(obj->name.c_str(), flags);

    if (ImGui::IsItemClicked()) {
        scene.selectedObject = obj;
        clickedOnItem = true;
    }

    // Clic droit sur cet objet
    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Supprimer le GameObject")) {
            objectToDelete = obj;
        }
        if (ImGui::MenuItem("Créer un enfant")) {
            auto child = std::make_shared<GameObject>("New Child");
            child->AddComponent(std::make_shared<TransformComponent>());
            scene.AddGameObject(child);
            child->SetParent(obj);
        }
        ImGui::EndPopup();
    }

    if (open) {
        for (auto& child : obj->children) {
            DrawGameObjectNode(scene, child, clickedOnItem, objectToDelete);
        }
        ImGui::TreePop();
    }

    ImGui::PopID();
}



