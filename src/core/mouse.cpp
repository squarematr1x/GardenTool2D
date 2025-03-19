#include "mouse.hpp"

#include "render-window.hpp"
#include "math/vec2.hpp"

namespace mouse
{

Vec2 getPosition(const RenderWindow& window) {
    const auto pos = sf::Mouse::getPosition(window.getWindow());
    return Vec2(pos.x, pos.y);
}

} // namespace Mouse
