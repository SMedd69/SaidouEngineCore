#include "Ui/Tools/ContextMenu.h"
#include <imgui.h>
#include <Objects/GameObject.h>
#include <UI/HierarchyWindow.h>
#include <Objects/TransformComponent.h>
#include <iostream>

namespace UI {

std::vector<MenuItem> ContextMenu::s_menuItems;

void ContextMenu::Begin(const std::string& popupId)
{
    if (ImGui::BeginPopup(popupId.c_str()))
    {
        for (auto& item : s_menuItems)
        {
            if (ImGui::MenuItem(item.label.c_str()))
                item.action();
        }
        s_menuItems.clear();
        ImGui::EndPopup();
    }
}

void ContextMenu::End() { }

void ContextMenu::AddMenuItem(const std::string& label, std::function<void()> action)
{
    s_menuItems.push_back({label, action});
}

void ContextMenu::ShowForGameObject(std::shared_ptr<GameObject> obj, Scene& scene,
                                    std::shared_ptr<GameObject>& objectToDelete,
                                    HierarchyWindow& window)
{
    if (ImGui::BeginPopup(("ItemContext##" + std::to_string(obj->id)).c_str()))
    {
        AddMenuItem("Renommer", [&]() {
            window.SetRenameTarget(obj);
        });

        AddMenuItem("Supprimer", [&]() {
            objectToDelete = obj;
        });

        AddMenuItem("Créer un enfant", [&]() {
            auto child = std::make_shared<GameObject>(window.MakeUniqueName(scene, "New Child"));
            child->AddComponent(std::make_shared<TransformComponent>());
            scene.AddGameObject(child);
            child->SetParent(obj);
        });

        for (auto& item : s_menuItems)
        {
            if (ImGui::MenuItem(item.label.c_str()))
                item.action();
        }

        s_menuItems.clear();
        ImGui::EndPopup();
    }
}

} // namespace UI
