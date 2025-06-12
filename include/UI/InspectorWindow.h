#pragma once
#include <UI/IUIWindow.h>
#include <Core/Scene.h>

class InspectorWindow : public IUIWindow
{
    public:
        Scene* m_scene = nullptr;

        void SetScene(Scene* s) { m_scene = s; }
        void Render() override;
        void DrawInspector(Scene& scene);
};