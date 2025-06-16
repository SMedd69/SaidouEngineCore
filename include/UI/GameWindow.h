#pragma once
#include <UI/IUIWindow.h>

class Scene;
class GameWindow : public IUIWindow
{
    public:
    void SetScene(Scene* scene) { m_scene = scene; }
    void Render() override;
private:
    Scene* m_scene = nullptr;
};