#include "Ui/Tools/ContextMenu.h"
#include <imgui.h>
#include <Objects/GameObject.h>
#include <UI/HierarchyWindow.h>
#include <Objects/TransformComponent.h>
#include <iostream>
#include <Engine/MeshFactory.h>
#include <Objects/MeshFilter.h>
#include <Objects/BoxCollider.h>
#include <Objects/MeshRenderer.h>
#include <Objects/DirectionnalLightComponent.h>
#include <Objects/CameraComponent.h>

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

        if (ImGui::BeginMenu("Objects"))
        {
            if (ImGui::BeginMenu("3D"))
            {

                if(ImGui::MenuItem("Créer un Cube")){
                    auto childCube = std::make_shared<GameObject>(window.MakeUniqueName(scene, "Cube"));
                    childCube->AddComponent(std::make_shared<TransformComponent>());

                    auto mesh = MeshFactory::CreatePrimitive(PrimitiveType::Cube);
                    auto meshFilter = std::make_shared<MeshFilter>(mesh);
                    meshFilter->meshName = PrimitiveTypeToString(PrimitiveType::Cube); // 🔄 synchronise le nom
                    childCube->AddComponent(meshFilter);

                    childCube->AddComponent(std::make_shared<MeshRenderer>());
                    childCube->AddComponent(std::make_shared<BoxCollider>());
                    scene.AddGameObject(childCube);
                    childCube->SetParent(obj);
                }

                if(ImGui::MenuItem("Créer un Plane")) {
                    auto plane = std::make_shared<GameObject>(window.MakeUniqueName(scene, "Plane"));
                    plane->AddComponent(std::make_shared<TransformComponent>());

                    auto mesh = MeshFactory::CreatePrimitive(PrimitiveType::Plane);
                    auto meshFilter = std::make_shared<MeshFilter>(mesh);
                    meshFilter->meshName = PrimitiveTypeToString(PrimitiveType::Plane); // 🔄 synchronise le nom
                    plane->AddComponent(meshFilter);

                    plane->AddComponent(std::make_shared<MeshRenderer>());
                    plane->AddComponent(std::make_shared<BoxCollider>());
                    scene.AddGameObject(plane);
                    plane->SetParent(obj);
                }

                if(ImGui::MenuItem("Créer une Sphere")) {
                    auto sphere = std::make_shared<GameObject>(window.MakeUniqueName(scene, "Sphere"));
                    sphere->AddComponent(std::make_shared<TransformComponent>());

                    auto mesh = MeshFactory::CreatePrimitive(PrimitiveType::Sphere);
                    auto meshFilter = std::make_shared<MeshFilter>(mesh);
                    meshFilter->meshName = PrimitiveTypeToString(PrimitiveType::Sphere); // 🔄 synchronise le nom
                    sphere->AddComponent(meshFilter);

                    sphere->AddComponent(std::make_shared<MeshRenderer>());
                    sphere->AddComponent(std::make_shared<BoxCollider>());
                    scene.AddGameObject(sphere);
                    sphere->SetParent(obj);
                }

                if(ImGui::MenuItem("Créer une Capsule")) {
                    auto capsule = std::make_shared<GameObject>(window.MakeUniqueName(scene, "Capsule"));
                    capsule->AddComponent(std::make_shared<TransformComponent>());

                    auto mesh = MeshFactory::CreatePrimitive(PrimitiveType::Capsule);
                    auto meshFilter = std::make_shared<MeshFilter>(mesh);
                    meshFilter->meshName = PrimitiveTypeToString(PrimitiveType::Capsule); // 🔄 synchronise le nom
                    capsule->AddComponent(meshFilter);

                    capsule->AddComponent(std::make_shared<MeshRenderer>());
                    capsule->AddComponent(std::make_shared<BoxCollider>());
                    scene.AddGameObject(capsule);
                    capsule->SetParent(obj);
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Lights"))
            {
                if(ImGui::MenuItem("Créer une DirectionnalLight")) {
                    auto directionnalLight = std::make_shared<GameObject>(window.MakeUniqueName(scene, "DirectionnalLight"));
                    directionnalLight->AddComponent(std::make_shared<TransformComponent>());
                    directionnalLight->AddComponent(std::make_shared<DirectionalLightComponent>());

                    scene.AddGameObject(directionnalLight);
                    directionnalLight->SetParent(obj);
                }

                ImGui::EndMenu();
            }

            if(ImGui::MenuItem("Créer une Caméra")) {
                auto camera = std::make_shared<GameObject>(window.MakeUniqueName(scene, "Camera"));
                camera->AddComponent(std::make_shared<TransformComponent>());
                camera->AddComponent(std::make_shared<CameraComponent>());

                scene.AddGameObject(camera);
                camera->SetParent(obj);
            }
            ImGui::EndMenu();
        }

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
