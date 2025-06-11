#pragma once

class IUIWindow
{
    public:
        virtual ~IUIWindow() = default;
        virtual void Render() = 0;
};