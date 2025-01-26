#pragma once

template <typename T>
struct Rect {
    T width;
    T height;
    T left;
    T top;

    Rect(T w_in, T h_in, T left_in, T top_in)
        : width(w_in), height(h_in), left(left_in), top(top_in)
    {
    }
};
