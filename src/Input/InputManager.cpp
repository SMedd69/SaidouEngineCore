#include "Input/InputManager.h"
#include <iostream>

#include <imgui_impl_glfw.h>

InputManager& InputManager::Instance()
{
    static InputManager instance;
    return instance;
}

void InputManager::Init(GLFWwindow* window)
{
    m_window = window;
    glfwSetWindowUserPointer(window, this);

    glfwSetScrollCallback(window, [](GLFWwindow* win, double xoff, double yoff) {
        ImGui_ImplGlfw_ScrollCallback(win, xoff, yoff);
        auto* self = static_cast<InputManager*>(glfwGetWindowUserPointer(win));
        InputEvent e;
        e.type = InputEventType::Scroll;
        e.scroll = { xoff, yoff };
        std::cout << "Scroll Y: " << yoff << std::endl;
        self->PushEvent(e);
    });

    glfwSetCursorPosCallback(window, [](GLFWwindow* win, double x, double y) {
        ImGui_ImplGlfw_CursorPosCallback(win, x, y);
        auto* self = static_cast<InputManager*>(glfwGetWindowUserPointer(win));
        InputEvent e;
        e.type = InputEventType::MouseMove;
        e.mouseMove = { x, y };
        self->PushEvent(e);
    });

    glfwSetMouseButtonCallback(window, [](GLFWwindow* win, int button, int action, int mods) {
        ImGui_ImplGlfw_MouseButtonCallback(win, button, action, mods);
        auto* self = static_cast<InputManager*>(glfwGetWindowUserPointer(win));
        InputEvent e;
        e.type = InputEventType::MouseButton;
        e.mouseButton = { button, action, mods };
        std::cout << "Mouse: " << e.mouseButton.button << std::endl;
        self->PushEvent(e);
    });
    
    glfwSetKeyCallback(window, [](GLFWwindow* win, int key, int scancode, int action, int mods) {
        ImGui_ImplGlfw_KeyCallback(win, key, scancode, action, mods);
        auto* self = static_cast<InputManager*>(glfwGetWindowUserPointer(win));
        InputEvent e;
        e.type = InputEventType::Key;
        e.key = { key, scancode, action, mods };
        std::cout << "Key: " << key << std::endl;
        self->PushEvent(e);
    });

    glfwSetCharCallback(window, [](GLFWwindow* win, unsigned int codepoint) {
        ImGui_ImplGlfw_CharCallback(win, codepoint);
        auto* self = static_cast<InputManager*>(glfwGetWindowUserPointer(win));
        InputEvent e;
        e.type = InputEventType::Char;
        e.character = { codepoint };
        self->PushEvent(e);
    });
}

void InputManager::PushEvent(const InputEvent& event) {
    m_eventQueue.push(event);
}

void InputManager::Update() {
    m_currentFrameEvents.clear();
    while (!m_eventQueue.empty()) {
        m_currentFrameEvents.push_back(m_eventQueue.front());
        m_eventQueue.pop();
    }
}

void InputManager::ClearEvents() {
    m_currentFrameEvents.clear();
}


const std::vector<InputEvent>& InputManager::GetEvents() const {
    return m_currentFrameEvents;
}