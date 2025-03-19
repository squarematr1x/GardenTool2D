#pragma once

#include <SFML/Window.hpp>

class RenderWindow;
class Vec2;

namespace mouse
{

    enum Wheel {
        VerticalWheel,
        HorizontalWheel
    };

    enum Button {
        Left,
        Right,
        Middle,
        XButton1,
        XButton2,

        ButtonCount
    };

    Vec2 getPosition(const RenderWindow& window);

} // namespace mouse
