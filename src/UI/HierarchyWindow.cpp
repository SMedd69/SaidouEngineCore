#include <UI/HierarchyWindow.h>
#include <Objects/TransformComponent.h>
#include <Objects/BoxCollider.h>
#include <Objects/MeshRenderer.h>
#include <Objects/MeshFilter.h>
#include <imgui.h>
#include <iostream>
#include <cstring>
#include <UI/Tools/ContextMenu.h>
#include <Objects/GameObject.h>
#include <Core/Scene.h>

void HierarchyWindow::Render()
{
    if (m_scene)
        DrawHierarchy(*m_scene);
    else
        std::cout << "Pas de scène\n";
}

void HierarchyWindow::DrawHierarchy(Scene& scene)
{
    ImGui::Begin("Hierarchy");

    bool clickedOnItem = false;
    std::shared_ptr<GameObject> objectToDelete = nullptr;

    for (auto& obj : scene.gameObjects)
    {
        if (obj->parent.expired())
        {
            DrawGameObjectNode(scene, obj, clickedOnItem, objectToDelete);
        }
    }

    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0) && !clickedOnItem)
        scene.selectedObject = nullptr;

    if (!scene.selectedObject ) {
        if (ImGui::BeginPopupContextWindow("HierarchyContext", ImGuiPopupFlags_MouseButtonRight)) {
            UI::ContextMenu::AddMenuItem("Créer un GameObject vide", [&]() {
                auto go = std::make_shared<GameObject>(MakeUniqueName(scene, "New GameObject"));
                go->AddComponent(std::make_shared<TransformComponent>());
                scene.AddGameObject(go);
            });

            UI::ContextMenu::AddMenuItem("Créer un Cube", [&]() {
                auto cube = std::make_shared<GameObject>(MakeUniqueName(scene, "Cube"));
                cube->AddComponent(std::make_shared<TransformComponent>());
                cube->AddComponent(std::make_shared<MeshFilter>());
                cube->AddComponent(std::make_shared<MeshRenderer>());
                cube->AddComponent(std::make_shared<BoxCollider>());
                scene.AddGameObject(cube);
            });

            for (auto& item : UI::ContextMenu::s_menuItems) {
                if (ImGui::MenuItem(item.label.c_str()))
                    item.action();
            }
            UI::ContextMenu::s_menuItems.clear();
            ImGui::EndPopup();
        }
    }



    if (scene.selectedObject)
    {
        if (ImGui::IsKeyPressed(ImGuiKey_F2))
            SetRenameTarget(scene.selectedObject);

        if (ImGui::IsKeyPressed(ImGuiKey_Delete))
            objectToDelete = scene.selectedObject;
    }

    if (objectToDelete)
    {
        if (scene.selectedObject == objectToDelete)
            scene.selectedObject = nullptr;

        scene.RemoveGameObject(objectToDelete.get());
    }

    ImGui::End();
}

void HierarchyWindow::DrawGameObjectNode(
    Scene& scene, std::shared_ptr<GameObject> obj,
    bool& clickedOnItem, std::shared_ptr<GameObject>& objectToDelete)
{
    ImGui::PushID(obj->id);

    ImGuiTreeNodeFlags flags =
        (scene.selectedObject == obj ? ImGuiTreeNodeFlags_Selected : 0) |
        (obj->children.empty() ? ImGuiTreeNodeFlags_Leaf : 0) |
        ImGuiTreeNodeFlags_OpenOnArrow;

    bool isRenaming = (m_renamingObject == obj);
    bool open = false;

    if (!isRenaming)
    {
        open = ImGui::TreeNodeEx((obj->name + "##Label").c_str(), flags);
    }
    else
    {
        ImGui::Selectable("##Renaming", true);
        ImGui::SameLine();
        ImGui::PushItemWidth(150);

        std::string inputId = "##RenameInput_" + std::to_string(obj->id);
        bool enterPressed = ImGui::InputText(inputId.c_str(), m_renameBuffer, sizeof(m_renameBuffer), ImGuiInputTextFlags_EnterReturnsTrue);

        // Décaler le focus d'une frame
        if (m_pendingRenameFocus)
        {
            ImGui::SetKeyboardFocusHere();
            m_pendingRenameFocus = false;
        }

        if (enterPressed)
        {
            std::string newName = m_renameBuffer;
            if (!newName.empty())
            {
                newName = MakeUniqueName(scene, newName);
                obj->name = newName;
            }
            m_renamingObject = nullptr;
        }

        if(ImGui::IsKeyPressed(ImGuiKey_Escape))
        {
            m_renamingObject = nullptr;
        }

        if (!ImGui::IsItemActive() && !ImGui::IsItemHovered() && !ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows))
            m_renamingObject = nullptr;


        ImGui::PopItemWidth();
    }

    if (ImGui::IsItemClicked())
    {
        scene.selectedObject = obj;
        clickedOnItem = true;
    }

    if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
        ImGui::OpenPopup(("ItemContext##" + std::to_string(obj->id)).c_str());
        scene.selectedObject = obj;
    }
    UI::ContextMenu::ShowForGameObject(obj, scene, objectToDelete, *this);

    if (open)
    {
        for (auto& child : obj->children)
            DrawGameObjectNode(scene, child, clickedOnItem, objectToDelete);

        ImGui::TreePop();
    }

    ImGui::PopID();
}

std::string HierarchyWindow::MakeUniqueName(Scene& scene, const std::string& baseName)
{
    std::string finalName = baseName;
    int count = 1;

    auto nameExists = [&](const std::string& name) {
        for (const auto& go : scene.gameObjects)
        {
            if (go->name == name)
                return true;
        }
        return false;
    };

    while (nameExists(finalName))
        finalName = baseName + " (" + std::to_string(count++) + ")";

    return finalName;
}

void HierarchyWindow::SetRenameTarget(std::shared_ptr<GameObject> obj)
{
    m_renamingObject = obj;
    strncpy(m_renameBuffer, obj->name.c_str(), sizeof(m_renameBuffer));
    m_renameBuffer[sizeof(m_renameBuffer) - 1] = '\0';

    // On attend une frame avant de donner le focus
    m_pendingRenameFocus = true;
}
