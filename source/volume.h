/*!
 *
 *	CS 595 Assignment 10
 *	Jie Jiang
 *	
 */
#ifndef	__VOLUME_H
#define __VOLUME_H

#include "vec.h"

namespace AJParallelRendering {

	class Renderer;

	class Volume
	{
	public:
		Volume(int *size);
		Volume(int x, int y, int z);
		Volume(int x, int y, int z, float *val);
		~Volume();
		
		void setSize(int x, int y, int z);
		void getSize(int &x, int &y, int &z);
		void getSize(int *size);

		bool setVolume(int x, int y, int z, float val);
		float getVolume(int x, int y, int z);
		float getVolume(int *xyz);
		float getVolume(const Vec3i &vec);
		bool createSubVolume(	int x, int y, int z, 
			 					int sizeX, int sizeY, int sizeZ,
								Volume *volume);
		void normalize(float min, float max);
		void updateMetaInfo();
		
	//protected:
		int _x;
		int _y;
		int _z;
		
		float _max;
		float _min;
		
		bool isWithinRange( int x, int y, int z);
		float *_valArray;
		
	private:
		float interpolate(float val, float min, float max);
		
		friend class Renderer;
	};
	
};

#endif // __VOLUME_H
