#pragma once

enum class InputEventType {
    MouseMove,
    MouseButton,
    Scroll,
    Key,
    Char
};

struct InputEvent {
    InputEventType type;

    union
    {
        struct { double x, y; } mouseMove;
        struct { int button, action, mods; } mouseButton;
        struct { double xOffset, yOffset; } scroll;
        struct { int key, scancode, action, mods; } key;
        struct { unsigned int codepoint; } character;
    };
};