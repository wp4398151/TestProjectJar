////////////////////////////////////////////////////
// implement some math helper function and class which is not
// in cmath.
// Wupeng 
// qq:4398151
////////////////////////////////////////////////////

#include "MathLite.h"
#include <math.h>
#include "Helper.h"

Vec3::Vec3(const Vec3 &v) :m_x(v.m_x), m_y(v.m_y), m_z(v.m_z)
{
}

Vec3::~Vec3()
{
}

void Vec3::operator=(const Vec3 &v)
{
	m_x = v.m_x;
	m_y = v.m_y;
	m_z = v.m_z;
}

Vec3 Vec3::operator+(const Vec3 &v)
{
	return Vec3(m_x + v.m_x, m_y + v.m_y, m_z + v.m_z);
}

Vec3 Vec3::operator-(const Vec3 &v)
{
	return Vec3(m_x - v.m_x, m_y - v.m_y, m_z - v.m_z);
}

Vec3 Vec3::operator/(const Vec3 &v)
{
	if (fabsf(v.m_x) <= EPSILON || fabsf(v.m_y) <= EPSILON || fabsf(v.m_z) <= EPSILON)
	{
		DOLOG("ERROR!");
		return *this;
	}
	return Vec3(m_x / v.m_x, m_y / v.m_y, m_z / v.m_z);
}

Vec3 Vec3::operator*(const Vec3 &v)
{
	return Vec3(m_x*v.m_x, m_y*v.m_y, m_z*v.m_z);
}

Vec3 Vec3::operator+(float f)
{
	return Vec3(m_x + f, m_y + f, m_z + f);
}

Vec3 Vec3::operator-(float f)
{
	return Vec3(m_x - f, m_y - f, m_z - f);
}

Vec3 Vec3::operator/(float f)
{
	if(fabsf(f) <= EPSILON)
	{
		return Vec3(0, 0, 0);
	}
	return Vec3(m_x / f, m_y / f, m_z / f);
}

Vec3 Vec3::operator*(float f)
{
	return Vec3(m_x*f, m_y*f, m_z*f);
}

float Vec3::dot(const Vec3 &v)
{
	return m_x*v.m_x + m_y*v.m_y + m_z*v.m_z;
}

float Vec3::length()
{
	return sqrtf(dot(*this));
}

void Vec3::normalize()
{
	float len = length();
	if (len < EPSILON) len = 1;
	len = 1 / len;

	m_x *= len;
	m_y *= len;
	m_z *= len;
}

Vec3 Vec3::cross(const Vec3 &v)
{
	return Vec3(m_y * v.m_z - m_z * v.m_y, m_z * v.m_x - m_x * v.m_z, m_x * v.m_y - m_y * v.m_x);
}


Vec2::Vec2(const Vec2 &v) :m_x(v.m_x), m_y(v.m_y)
{
}

Vec2::~Vec2()
{
}

void Vec2::operator=(const Vec2 &v)
{
	m_x = v.m_x;
	m_y = v.m_y;
}

Vec2 Vec2::operator+(const Vec2 &v)
{
	return Vec2(m_x + v.m_x, m_y + v.m_y);
}

Vec2 Vec2::operator-(const Vec2 &v)
{
	return Vec2(m_x - v.m_x, m_y - v.m_y);
}

Vec2 Vec2::operator/(const Vec2 &v)
{
	if (fabsf(v.m_x) <= EPSILON || fabsf(v.m_y) <= EPSILON)
	{
		DOLOG("ERROR!");
		return *this;
	}
	return Vec2(m_x / v.m_x, m_y / v.m_y);
}

Vec2 Vec2::operator*(const Vec2 &v)
{
	return Vec2(m_x*v.m_x, m_y*v.m_y);
}

Vec2 Vec2::operator+(float f)
{
	return Vec2(m_x + f, m_y + f);
}

Vec2 Vec2::operator-(float f)
{
	return Vec2(m_x - f, m_y - f);
}

Vec2 Vec2::operator/(float f)
{
	if (fabsf(f) <= EPSILON)
	{
		return Vec2(0, 0);
	}
	return Vec2(m_x / f, m_y / f);
}

Vec2 Vec2::operator*(float f)
{
	return Vec2(m_x*f, m_y*f);
}

float Vec2::dot(const Vec2 &v)
{
	return m_x*v.m_x + m_y*v.m_y;
}

float Vec2::length()
{
	return sqrtf(dot(*this));
}

void Vec2::normalize()
{
	float len = length();
	if (len < EPSILON) len = 1;
	len = 1 / len;

	m_x *= len;
	m_y *= len;
}