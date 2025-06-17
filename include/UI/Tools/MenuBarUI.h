#pragma once

class MenuBarUI
{
    public:
        bool* p_showLauncher = nullptr;
        bool* p_showGrid = nullptr;

        void Render();
};