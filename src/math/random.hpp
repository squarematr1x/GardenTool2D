#pragma once

#include <random>
#include <iterator>

namespace math
{

template<typename T, typename TGenerator>
T selectRandom(T start, T end, TGenerator& gen) {
    std::uniform_int_distribution<> dis(0, static_cast<int>(std::distance(start, end)) - 1);
    std::advance(start, dis(gen));

    return start;
}

template<typename T>
T getRandomElement(T start, T end) {
    static std::random_device rd;
    static std::mt19937 gen(rd());

    return selectRandom(start, end, gen);
}

} // namespace math
