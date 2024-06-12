#include "engine.hpp"

#include "util/profiler.hpp"

int main() {
    PROFILE_FUNCTION();

    GameEngine game("config/assets.txt");
    game.run();

    return 0;
}
