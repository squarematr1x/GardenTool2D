#pragma once

#include <SFML/Window.hpp>

class Event {
    sf::Event m_event;

public:
    enum EventType {
        Closed,
        Resized,
        LostFocus,
        GainedFocus,
        TextEntered,
        KeyPressed,
        KeyReleased,
        MouseWheelMoved,
        MouseWheelScrolled,
        MouseButtonPressed,
        MouseButtonReleased,
        MouseMoved,
        MouseEntered,
        MouseLeft,
        JoystickButtonPressed,
        JoystickButtonReleased,
        JoystickMoved,
        JoystickConnected,
        JoystickDisconnected,
        TouchBegan,
        TouchMoved,
        TouchEnded,
        SensorChanged,
        Count
    };

    sf::Event& getEvent() { return m_event; }

    EventType getType() { return static_cast<EventType>(m_event.type); }

    unsigned int getWidth() { return m_event.size.width; }
    unsigned int getHeight() { return m_event.size.height; }

    sf::Keyboard::Key getKeyCode() { return m_event.key.code; }
    sf::Mouse::Button getMouseButton() { return m_event.mouseButton.button; }
    sf::Mouse::Wheel getMouseWheelScrollWheel() { return m_event.mouseWheelScroll.wheel; }
    float getMouseWheelScrollDelta() { return m_event.mouseWheelScroll.delta; }
};
