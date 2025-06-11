#pragma once
#include "InputEvent.h"

#include <queue>
#include <vector>
#include <GLFW/glfw3.h>

class InputManager {
    public:
        static InputManager& Instance();

        void Init(GLFWwindow* window);
        void Update();

        const std::vector<InputEvent>& GetEvents() const;

    private:
        GLFWwindow* m_window = nullptr;
        std::queue<InputEvent> m_eventQueue;
        std::vector<InputEvent> m_currentFrameEvents;

        InputManager() = default;
        ~InputManager() = default;

        InputManager(const InputManager&) = delete;
        InputManager& operator=(const InputManager&) = delete;

        void PushEvent(const InputEvent& event);
};