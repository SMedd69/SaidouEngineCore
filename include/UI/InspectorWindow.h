#pragma once
#include <UI/IUIWindow.h>
#include <Engine/Material.h>
#include <Objects/ScriptComponent.h>
#include <Core/Scene.h>

class InspectorWindow : public IUIWindow
{
    public:
        Scene* m_scene = nullptr;
        std::shared_ptr<Material> m_selectedMaterial = nullptr;
        std::shared_ptr<ScriptComponent> m_selectedScript;

        void SetScene(Scene* s) { m_scene = s; }
        void Render() override;
        void DrawInspector(Scene& scene);

        void SetSelectedMaterial(std::shared_ptr<Material> mat) { m_selectedMaterial = mat; }
        void SetSelectedScript(std::shared_ptr<ScriptComponent> script) {
            m_selectedScript = script;
        }

};