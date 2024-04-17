#include "Vec2.hpp"

Vec2 Vec2::operator+ (const Vec2& p) const {
	return Vec2(x + p.x, y + p.y);
}

Vec2 Vec2::operator- (const Vec2& p) const {
	return Vec2(x - p.x, y - p.y);
}

Vec2 Vec2::operator* (const float scale) const {
	return Vec2(scale * x, scale * y);
}

Vec2 Vec2::operator/ (const float scale) const {
	return Vec2(x / scale, y / scale);
}

Vec2 Vec2::operator+= (const Vec2& p) {
	x += p.x;
	y += p.y;

	return *this;
}

Vec2 Vec2::operator-= (const Vec2& p) {
	x -= p.x;
	y -= p.y;

	return *this;
}

Vec2 Vec2::operator*= (const float scale) {
	x *= scale;
	y *= scale;

	return *this;
}

Vec2 Vec2::operator/= (const float scale) {
	x /= scale;
	y /= scale;

	return *this;
}

bool Vec2::operator== (const Vec2& p) const {
	return x == p.x && y == p.y;
}

bool Vec2::operator!= (const Vec2& p) const {
	return !(*this == p);
}

Vec2& Vec2::scale(const float scale) {
	x *= scale; y *= scale; return *this;
}

Vec2& Vec2::add(const float value) {
	x += value; y += value; return *this;
}

std::ostream& operator<< (std::ostream& out, const Vec2& p) {
	out << "(" << p.x << ", " << p.y << ")";

	return out;
}

float Vec2::distance(const Vec2& p) const {
	const float dx{ x - p.x };
	const float dy{ y - p.y };

	return sqrtf(dx*dx + dy*dy);
}

float Vec2::length() const {
	return sqrtf(x*x + y*y);
}

Vec2 Vec2::normalize() const {
	return *this / sqrtf(x*x + y*y);
}
