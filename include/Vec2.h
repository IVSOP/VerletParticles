#ifndef VEC2_H
#define VEC2_H

#include <cmath>

typedef double vec2number;

struct Vec2 {
	vec2number x, y;

	// allows to be made at compile time
	constexpr Vec2()
		: x(0), y(0)
		{}
	
	constexpr Vec2(vec2number x, vec2number y)
		: x(x), y(y)
		{}

	Vec2(const Vec2& v)
		: x(v.x), y(v.y)
		{}

	// Vec2(Vec2& v)
	// 	: x(v.x), y(v.y)
	// 	{}

	/*constexpr ????*/ Vec2& operator=(const Vec2& v) {
		x = v.x;
		y = v.y;
		return *this;
	}


// operations
	constexpr Vec2 operator+(const Vec2& v) const {
		return Vec2(x + v.x, y + v.y);
	}

	constexpr Vec2 operator-(const Vec2& v) const{
		return Vec2(x - v.x, y - v.y);
	}


// operations with assignment
	Vec2& operator+=(const Vec2& v) {
		x += v.x;
		y += v.y;
		return *this;
	}

	Vec2& operator-=(const Vec2& v) {
		x -= v.x;
		y -= v.y;
		return *this;
	}


// operations with numbers
	constexpr Vec2 operator+(vec2number s) const {
		return Vec2(x + s, y + s);
	}

	constexpr Vec2 operator-(vec2number s) const {
		return Vec2(x - s, y - s);
	}

	constexpr Vec2 operator*(vec2number s) const {
		return Vec2(x * s, y * s);
	}

	constexpr Vec2 operator/(vec2number s) const {
		return Vec2(x / s, y / s);
	}
	


// operations with numbers and assignment
	Vec2& operator+=(vec2number s) {
		x += s;
		y += s;
		return *this;
	}

	Vec2& operator-=(vec2number s) {
		x -= s;
		y -= s;
		return *this;
	}

	Vec2& operator*=(vec2number s) {
		x *= s;
		y *= s;
		return *this;
	}

	Vec2& operator/=(vec2number s) {
		x /= s;
		y /= s;
		return *this;
	}


// math
	vec2number dist(Vec2 v) const {
		Vec2 d(v.x - x, v.y - y);
		return d.length();
	}

	constexpr vec2number length() const {
		return std::sqrt(x * x + y * y);
	}
};

#endif
