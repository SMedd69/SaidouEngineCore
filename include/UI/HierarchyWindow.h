#pragma once
#include <UI/IUIWindow.h>
#include <Core/Scene.h>
#include <memory>

namespace UI {
    class ContextMenu;
}

class HierarchyWindow : public IUIWindow
{
    public:
        Scene* m_scene = nullptr;

        void SetScene(Scene* s) {m_scene = s; }
        void Render() override;

        void DrawHierarchy(Scene& scene);

        void DrawGameObjectNode(Scene& scene, std::shared_ptr<GameObject> obj,
                        bool& clickedOnItem, std::shared_ptr<GameObject>& objectToDelete);
        void SetRenameTarget(std::shared_ptr<GameObject> obj);

        std::string MakeUniqueName(Scene& scene, const std::string& baseName);
    private:
            std::shared_ptr<GameObject> m_renamingObject = nullptr;
            char m_renameBuffer[256] = { 0 };

            bool m_focusRename;
            bool m_pendingRenameFocus = false;

};