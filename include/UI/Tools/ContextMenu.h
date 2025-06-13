#pragma once

#include <memory>
#include <functional>
#include <vector>
#include <string>

namespace ImGui { struct ImGuiContext; }

class GameObject;
class Scene;
class HierarchyWindow;

namespace UI {

struct MenuItem {
    std::string label;
    std::function<void()> action;
};

class ContextMenu {
public:
    static void Begin(const std::string& popupId);
    static void End();
    static void AddMenuItem(const std::string& label, std::function<void()> action);

    static void ShowForGameObject(
        std::shared_ptr<GameObject> obj, Scene& scene,
        std::shared_ptr<GameObject>& objectToDelete,
        HierarchyWindow& window);
        
    static std::vector<MenuItem> s_menuItems;
private:
};

} // namespace UI