/*!
 *
 *	CS 595 Assignment 10
 *	Jie Jiang
 *	
 */

#include "volume.h"
#include <cstring>
#include <iostream>
#include <cstdlib>

using namespace AJParallelRendering;
using namespace std;

Volume::Volume (int *size) : _x(size[0]), _y(size[1]), _z(size[2])
{
	_valArray = (float *) malloc ( _x * _y * _z *sizeof(float));
}


Volume::Volume (int x, int y, int z) : _x(x), _y(y), _z(z)
{
	_valArray = (float *) malloc ( x * y * z *sizeof(float));
}

Volume::Volume (int x, int y, int z, float *val) : _x(x), _y(y), _z(z)
{
	_valArray = (float *) malloc ( x * y * z *sizeof(float));
	memcpy( _valArray, val, x*y*z*sizeof(float) );
}

Volume::~Volume()
{
	free(_valArray);
}

void Volume::setSize(int x, int y, int z)
{
	_x = x;
	_y = y;
	_z = z;
	if ( x*y*z > sizeof(_valArray))
	{
		free(_valArray);
		_valArray = (float*) malloc ( x*y*z*sizeof(float));
	}
}

void Volume::getSize(int &x, int &y, int &z)
{
	x = _x;
	y = _y;
	z = _z;
}

void Volume::getSize(int *size)
{
	size[0] = _x;
	size[1] = _y;
	size[2] = _z;
}

bool Volume::setVolume(int x, int y, int z, float val)
{
	if ( isWithinRange(x,y,z) )
	{
		int index = x + _x * ( y + _y * z);
		_valArray[index] = val;
		return true;
	} else {
		cout << "setVolume input: <" << x << ", " << y << ", " << z << "> out of range: <" << _x << ", " << _y << ", " << _z << ">" << endl; 
		return false;
	}
}

float Volume::getVolume(int x, int y, int z)
{
	if ( isWithinRange(x,y,z) )
	{
		int index = x + _x * ( y + _y * z);
		return _valArray[index];
	} else {
		cout << "getVolume input: <" << x << ", " << y << ", " << z << "> out of range: <" << _x << ", " << _y << ", " << _z << ">" << endl; 
		return 0;
	}
}

float Volume::getVolume(int *xyz)
{
	return getVolume(xyz[0], xyz[1], xyz[2]);
}

float Volume::getVolume(const Vec3i &vec)
{
	return getVolume(vec._x, vec._y, vec._z);
}

bool Volume::createSubVolume(	int x, int y, int z, 
						int sizeX, int sizeY, int sizeZ,
						Volume *volume)
{
	if ( !isWithinRange(x, y, z) || !isWithinRange(x+sizeX, y+sizeY, z+sizeZ) )
	{
		cout << "createSubVolume size out of range" << endl;
		return false;
	}
	
	volume->setSize(sizeX, sizeY, sizeZ);
	for ( int u = 0; u < sizeX; u++)
	{
		for ( int v = 0; v < sizeY; v++)
		{
			for ( int w = 0; w < sizeZ; w++)
			{
				int subIndex = u + sizeX * ( v + sizeY * w );
				int index = (x+u) + sizeX * ( (y+v) + sizeY * (z+w) );
				volume->_valArray[subIndex] = _valArray[index];
			}	
		}	
	}
	return true;
}

void Volume::updateMetaInfo()
{
	_max = _valArray[0];
	_min = _valArray[0];
	for ( int index = 0; index < _x * _y * _z; index++)
	{
		if (_valArray[index] > _max)
		{
			_max = _valArray[index];
			continue;
		}
		if (_valArray[index] < _min)
		{
			_min = _valArray[index];
			continue;
		}
	}
}

bool Volume::isWithinRange(int x, int y, int z)
{
	if ( x >= 0 && x < _x && y >= 0 && y < _y && z >= 0 && z < _z)
	{
		return true;
	} else {
		return false;
	}
}

void Volume::normalize(float min, float max)
{
	int size = _x*_y*_z;
	for ( int i = 0; i < size; i++ )
	{
		_valArray[i] = interpolate(_valArray[i], min, max);
	}
}

float Volume::interpolate(float val, float min, float max)
{
	return (val - min) / (max - min);
}
