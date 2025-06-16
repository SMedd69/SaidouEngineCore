#pragma once
#include <UI/IUIWindow.h>
#include <Engine/Material.h>
#include <Core/Scene.h>

class InspectorWindow : public IUIWindow
{
    public:
        Scene* m_scene = nullptr;
        std::shared_ptr<Material> m_selectedMaterial = nullptr;

        void SetScene(Scene* s) { m_scene = s; }
        void Render() override;
        void DrawInspector(Scene& scene);

        void SetSelectedMaterial(std::shared_ptr<Material> mat) { m_selectedMaterial = mat; }
};