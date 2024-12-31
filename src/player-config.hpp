#pragma once

#include <string>

struct PlayerConfig {
    float x{ 0.0f };
    float y{ 0.0f };

    float bbox_x{ 0.0f };
    float bbox_y{ 0.0f };

    float v{ 0.0f };
    float max_v{ 0.0f };
    float jump_v{ 0.0f };
    float gravity{ 0.0f };
    
    std::string weapon{ "" };

    int hp{ 4 };
};
