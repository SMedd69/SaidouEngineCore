#pragma once
#include <UI/IUIWindow.h>
#include <Core/Scene.h>

class HierarchyWindow : public IUIWindow
{
    public:
        Scene* m_scene = nullptr;

        void SetScene(Scene* s) {m_scene = s; }
        void Render() override;

        void DrawHierarchy(Scene& scene);

        void DrawGameObjectNode(Scene& scene, std::shared_ptr<GameObject> obj,
                        bool& clickedOnItem, std::shared_ptr<GameObject>& objectToDelete);
};