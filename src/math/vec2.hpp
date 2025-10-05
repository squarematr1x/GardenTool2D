#pragma once

#include <iostream>

enum class Hearistic : unsigned char {
    MANHATTAN,
    EUCLIDIC
};

class Vec2 {
public:
	float x{ 0 };
	float y{ 0 };

	Vec2() 
	{
	}

	Vec2(float x_in, float y_in)
		: x(x_in), y(y_in)
	{
	}

	Vec2(float both)
		: x(both), y(both)
	{
	}

	Vec2(int x_in, int y_in)
		: x(static_cast<float>(x_in)), y(static_cast<float>(y_in))
	{
	}

	Vec2(int both)
		: x(static_cast<float>(both)), y(static_cast<float>(both))
	{
	}

	Vec2(unsigned int x_in, unsigned int y_in)
		: x(static_cast<float>(x_in)), y(static_cast<float>(y_in))
	{
	}

	Vec2(unsigned int both)
		: x(static_cast<float>(both)), y(static_cast<float>(both))
	{
	}

	Vec2(size_t x_in, size_t y_in)
		: x(static_cast<float>(x_in)), y(static_cast<float>(y_in))
	{
	}

	Vec2(size_t both)
		: x(static_cast<float>(both)), y(static_cast<float>(both))
	{
	}

	Vec2 operator+ (const Vec2& p) const;
	Vec2 operator- (const Vec2& p) const;
	Vec2 operator* (const float scale) const;
	Vec2 operator/ (const float scale) const;

	Vec2 operator+= (const Vec2& p);
	Vec2 operator-= (const Vec2& p);
	Vec2 operator*= (const float scale);
	Vec2 operator/= (const float scale);

	bool operator== (const Vec2& p) const;
	bool operator!= (const Vec2& p) const;
	bool operator< (const Vec2& p) const;

	Vec2& scale(const float scale);
	Vec2& add(const float value);

	float distance(const Vec2& p, Hearistic h = Hearistic::EUCLIDIC) const;
	float length() const;

	Vec2 normalize() const;

	friend std::ostream& operator<< (std::ostream& out, const Vec2& p);
};

struct Vec2Hasher {
    std::size_t operator()(const Vec2& v) const {
        std::size_t hx = std::hash<float>{}(v.x);
        std::size_t hy = std::hash<float>{}(v.y);
        return hx ^ (hy << 1);
    }
};
