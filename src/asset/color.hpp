#pragma once

using channel = unsigned char;

struct Color {
    channel r{ 255 };
    channel g{ 255 };
    channel b{ 255 };
    channel a{ 255 };

    Color() = default;

    Color(channel r_in, channel g_in, channel b_in, channel a_in=255)
        : r(r_in), g(g_in), b(b_in), a(a_in)
    {
    }
};
