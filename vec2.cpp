#include <cmath>
#include "vec2.h"


// vec2 member functions

vec2::vec2() {
	xy[0] = 0.f;
	xy[1] = 0.f;
	lengthCached = false;
}

vec2::vec2(float x, float y) {
	xy[0] = x;
	xy[1] = y;
	lengthCached = false;	
}

void vec2::set(float x, float y) {
	xy[0] = x;
	xy[1] = y;
	lengthCached = false;
}

float& vec2::operator[](const int index) {
	lengthCached = false;
	return xy[index];
}

const float& vec2::operator[](const int index) const {
	return xy[index];	
}

void vec2::negate() {
	lengthCached = false;
	xy[0] *= -1.f;
	xy[1] *= -1.f;
}

vec2& vec2::operator=(const vec2& v) {
	if(this == &v) return *this;
	lengthCached = false;
	xy[0] = v[0];
	xy[1] = v[1];
	return *this;
}
bool vec2::operator==(const vec2& v) {
	if(xy[0] == v[0] && xy[1] == v[1]) return true;
	return false;	
}

bool vec2::operator!=(const vec2& v) {
	return !(*this == v);
}

vec2& vec2::operator+=(const vec2& v) {
	lengthCached = false;
	xy[0] += v[0];
	xy[1] += v[1];
	return *this;
}

vec2& vec2::operator-=(const vec2& v) {
	lengthCached = false;
	vec2 tmp(v);
	tmp.negate();
	*this += tmp;
	return *this;
}

vec2& vec2::operator*=(const float& x) {
	lengthCached = false;
	xy[0] *= x;
	xy[1] *= x;
	return *this;
}

vec2 vec2::operator+(const vec2& v) {
	return vec2(*this) += v;
}

vec2 vec2::operator-(const vec2& v) {
	return vec2(*this) -= v;
}

vec2 vec2::operator*(const float& x) {
	return vec2(*this) *= x;
}

float vec2::dot_product(const vec2& v) {
	return (xy[0] * v[0] + xy[1] * v[1]);
}

float vec2::len() {
	if(!lengthCached) {
		C_length2 = xy[0] * xy[0] + xy[1] * xy[1];
		C_length = std::sqrt(C_length2);
		lengthCached = true;
	}
	return C_length;
}

float vec2::len2() {
	if(!lengthCached) {
		C_length2 = xy[0] * xy[0] + xy[1] * xy[1];
	}
	return C_length2;
}

void vec2::normalize() {
	float len = this->len();
	xy[0] /= len;
	xy[1] /= len;
	lengthCached = true;
	C_length2 = C_length = 1.f;
}


// vec2 interpolate methods

vec2 Vec2Interpolate::doHermite(vec2 a, vec2 b, vec2 a_minus_one, vec2 b_plus_one, float t) {
	// all bets are off if t > 1.f or t < 0.f
	float tpow2 = t * t;
	float tpow3 = tpow2 * t;
	float h1 = 2.f*tpow3 - 3.f*tpow2 + 1.f;
	float h2 = -2.f*tpow3 + 3.f*tpow3;
	float h3 = tpow3 - 2.f*tpow2 + t;
	float h4 = tpow3 - tpow2;
	return vec2(a*h1 + b*h2 + getTangentCardinal(a_minus_one, b, 0.5f)*h3 + getTangentCardinal(a, b_plus_one, 0.5f)*h4);
}

vec2 Vec2Interpolate::getTangentCardinal(vec2 prev, vec2 next, float a) {
	return vec2((next - prev) * a);
}

vec2 Vec2Interpolate::getTangentCatmullRom(vec2 prev, vec2 next) {
	return getTangentCardinal(prev, next, 0.5f);
}