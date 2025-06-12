#include <UI/HierarchyWindow.h>
#include <Objects/TransformComponent.h>
#include <imgui.h>
#include <iostream>

void HierarchyWindow::Render()
{
    if (m_scene)
    {
        std::cout << "Scene est ok! \n";
        DrawHierarchy(*m_scene);
    } else {
        std::cout << "Pas de scene\n";
    }
}

void HierarchyWindow::DrawHierarchy(Scene& scene) {
    ImGui::Begin("Hierarchy");

    // Affichage des GameObjects
    for (auto& obj : scene.gameObjects) {
        if (ImGui::Selectable(obj->name.c_str(), scene.selectedObject == obj)) {
            scene.selectedObject = obj;
        }

        // Clic droit sur un objet (popup par item si besoin plus tard)
    }

    // 👇 Clic droit dans la fenêtre
    if (ImGui::BeginPopupContextWindow(nullptr, ImGuiPopupFlags_MouseButtonRight)) {
        if (ImGui::MenuItem("Créer un GameObject vide")) {
            auto go = std::make_shared<GameObject>("New GameObject");
            go->AddComponent(std::make_shared<TransformComponent>());
            scene.AddGameObject(go);
        }

        ImGui::EndPopup();
    }

    ImGui::End();
}
