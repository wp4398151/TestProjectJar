////////////////////////////////////////////////////
// implement some math helper function and class which is not
// in cmath.
// Wupeng 
// qq:4398151
////////////////////////////////////////////////////

#pragma once  

#include <math.h>
/**
 * the different of two float
 */
#define EPSILON 1e-6

#define ISEQFLOAT(x, y)	\
	(fabs((x)-(y)) <= EPSILON)) ? true : false)

#define ISZEROFLOAT(x)	\
	(fabs(x)) <= EPSILON)) ? true : false)

class Vec3
{
public:
	Vec3() :m_x(0), m_y(0), m_z(0){}
	Vec3(float x, float y, float z) :m_x(x), m_y(y), m_z(z){}
	Vec3(const Vec3 &v);
	~Vec3();
	void operator=(const Vec3 &v);
	Vec3 operator+(const Vec3 &v);
	Vec3 operator-(const Vec3 &v);
	Vec3 operator/(const Vec3 &v);
	Vec3 operator*(const Vec3 &v);
	Vec3 operator+(float f);
	Vec3 operator-(float f);
	Vec3 operator/(float f);
	Vec3 operator*(float f);
	float dot(const Vec3 &v);
	float length();
	void normalize();
	Vec3 cross(const Vec3 &v);

public:
	float m_x;
	float m_y;
	float m_z;
};

class Vec2
{
public:
	Vec2() :m_x(0), m_y(0){}
	Vec2(float x, float y) :m_x(x), m_y(y){}
	Vec2(const Vec2 &v);
	~Vec2();
	void operator=(const Vec2 &v);
	Vec2 operator+(const Vec2 &v);
	Vec2 operator-(const Vec2 &v);
	Vec2 operator/(const Vec2 &v);
	Vec2 operator*(const Vec2 &v);
	Vec2 operator+(float f);
	Vec2 operator-(float f);
	Vec2 operator/(float f);
	Vec2 operator*(float f);
	float dot(const Vec2 &v);
	float length();
	void normalize();

public:
	float m_x;
	float m_y;
};
