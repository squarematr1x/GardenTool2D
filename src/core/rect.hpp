#pragma once

template <typename T>
struct Rect {
    T left;
    T top;
    T width;
    T height;

    Rect(T left_in, T top_in, T w_in, T h_in)
        : left(left_in), top(top_in), width(w_in), height(h_in)
    {
    }
};
