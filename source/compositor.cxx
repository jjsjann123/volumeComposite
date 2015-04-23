#include "compositor.h"
#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define IMAGE_COMPOSITION 0

using namespace AJParallelRendering;
using std::pow;

void Compositor::saveImage(const char* filename)
{
	_image->saveImage(filename);
}

void Compositor::setImage(ImageIO *image)
{
	_image = image;
}

void Compositor::composite(int id, int d)
{
positor::compositeArray( bool send, int commID, int offset, int length)
{
	int bufferLength = _image->getDataLength(length);
	if ( send )
	{
		MPI_Send( _image->getDataPointer(offset), bufferLength, MPI_FLOAT, commID, IMAGE_COMPOSITION, MPI_COMM_WORLD);  
	} else {
		MPI_Status status;
		MPI_Recv( _tempBuffer, bufferLength, MPI_FLOAT, commID, IMAGE_COMPOSITION, MPI_COMM_WORLD, &status);
		float *data;
		data = _image->getDataPointer(offset);
		for (int i = 0; i < length; i++ )
		{
			bool beneath;
			if ( _direction )
			{
				if ( _tempBuffer[i*5+4] > data[i*5+4] )
				{
					beneath = true;
				} else {
					beneath = false;
				}
			} else {
				if ( _tempBuffer[i*5+4] > data[i*5+4] )
				{
					beneath = false;
				} else {
					beneath = true;
				}
			}
			if ( beneath )
			{
				if (data[i*5+3] != 0)
				{
					//data[i*5] = data[i*5] + _tempBuffer[i*5] * _tempBuffer[i*5*3] * ( 1- data[i*5+3]);
					//data[i*5+1] = data[i*5+1] + _tempBuffer[i*5+1] * _tempBuffer[i*5+3] * ( 1- data[i*5+3]);
					//data[i*5+2] = data[i*5+2] + _tempBuffer[i*5+2] * _tempBuffer[i*5+3] * ( 1- data[i*5+3]);
					//data[i*5+3] = data[i*5+3] + _tempBuffer[i*5+3] * ( 1 - data[i*5+3]);
					data[i*5] = data[i*5] + _tempBuffer[i*5] * ( 1- data[i*5+3]);
					data[i*5+1] = data[i*5+1] + _tempBuffer[i*5+1] * ( 1- data[i*5+3]);
					data[i*5+2] = data[i*5+2] + _tempBuffer[i*5+2] * ( 1- data[i*5+3]);
					data[i*5+3] = data[i*5+3] + _tempBuffer[i*5+3] * ( 1 - data[i*5+3]);
					data[i*5+4] = _tempBuffer[i*5+4];
				} else {
					memcpy ( data+i*5, _tempBuffer+i*5, 4*sizeof(float));
				}
			} else {
				if (_tempBuffer[i*5+3] == 0)
					continue;
				//data[i*5] = _tempBuffer[i*5] + data[i*5] * data[i*5*3] * ( 1- _tempBuffer[i*5+3]);
				//data[i*5+1] = _tempBuffer[i*5+1] + data[i*5+1] * data[i*5+3] * ( 1- _tempBuffer[i*5+3]);
				//data[i*5+2] = _tempBuffer[i*5+2] + data[i*5+2] * data[i*5+3] * ( 1- _tempBuffer[i*5+3]);
				//data[i*5+3] = _tempBuffer[i*5+3] + data[i*5+3] * ( 1 - _tempBuffer[i*5+3]);
				data[i*5] = _tempBuffer[i*5] + data[i*5] * ( 1- _tempBuffer[i*5+3]);
				data[i*5+1] = _tempBuffer[i*5+1] + data[i*5+1] * ( 1- _tempBuffer[i*5+3]);
				data[i*5+2] = _tempBuffer[i*5+2] + data[i*5+2] * ( 1- _tempBuffer[i*5+3]);
				data[i*5+3] = _tempBuffer[i*5+3] + data[i*5+3] * ( 1 - _tempBuffer[i*5+3]);
			}
		}
	}
}
