#include "GameEngine.hpp"

#include "util/Profiler.hpp"

int main() {
    PROFILE_FUNCTION();

    GameEngine game("config/assets.txt");
    game.run();

    return 0;
}
