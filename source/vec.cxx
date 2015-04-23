#include "vec.h"

using namespace AJParallelRendering;

Vec3i Vec3i::operator + (const Vec3i & operand)
{
	Vec3i newVec = *this;
	newVec._x += operand._x;
	newVec._y += operand._y;
	newVec._z += operand._z;
	return *this;
}

Vec3i Vec3i::operator - (const Vec3i & operand)
{
	Vec3i newVec = *this;
	newVec._x -= operand._x;
	newVec._y -= operand._y;
	newVec._z -= operand._z;
	return *this;
}

Vec3i& Vec3i::operator += (const Vec3i & operand)
{
	this->_x += operand._x;
	this->_y += operand._y;
	this->_z += operand._z;
	return *this;
}

Vec3i& Vec3i::operator -= (const Vec3i & operand)
{
	this->_x -= operand._x;
	this->_y -= operand._y;
	this->_z -= operand._z;
	return *this;
}

int Vec3i::operator [] (int index)
{
	switch (index)
	{
	case 0:
		return _x;
	case 1:
		return _y;
	case 2:
		return _z;
	}
}

Vec3f Vec3f::operator + (const Vec3f & operand)
{
	Vec3f newVec = *this;
	newVec._x += operand._x;
	newVec._y += operand._y;
	newVec._z += operand._z;
	return *this;
}

Vec3f Vec3f::operator - (const Vec3f & operand)
{
	Vec3f newVec = *this;
	newVec._x -= operand._x;
	newVec._y -= operand._y;
	newVec._z -= operand._z;
	return *this;
}

Vec3f& Vec3f::operator += (const Vec3f & operand)
{
	this->_x += operand._x;
	this->_y += operand._y;
	this->_z += operand._z;
	return *this;
}

Vec3f& Vec3f::operator -= (const Vec3f & operand)
{
	this->_x -= operand._x;
	this->_y -= operand._y;
	this->_z -= operand._z;
	return *this;
}

float Vec3f::operator [] (int index)
{
	switch (index)
	{
	case 0:
		return _x;
	case 1:
		return _y;
	case 2:
		return _z;
	}
}

float Vec4f::operator [] (int index)
{
	switch (index)
	{
	case 0:
		return _x;
	case 1:
		return _y;
	case 2:
		return _z;
	case 3:
		return _a;
	}
}

void Vec4f::getValue(float *rgba)
{
	rgba[0] = _x;
	rgba[1] = _y;
	rgba[2] = _z;
	rgba[3] = _a;
}

void Vec4f::setValue(float *rgba)
{
	_x = rgba[0];
	_y = rgba[1];
	_z = rgba[2];
	_a = rgba[3];
}
