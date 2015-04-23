/*	CS 595 Assignment 10
 *	Jie Jiang
 *	
 */

#include "renderer.h"
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <stdio.h>

using namespace std;
using namespace AJParallelRendering;

ImageIO::ImageIO()
{
	_width = 0;
	_height = 0;
	_rgbadData = NULL;
}

ImageIO::ImageIO(const ImageIO &image)
{
	cout << "copying" << endl;
	initiateData(image._width, image._height, image._rgbadData);
}

ImageIO& ImageIO::operator = (const ImageIO &image)
{
	cout << "assign" << endl;
	initiateData(image._width, image._height, image._rgbadData);
}

ImageIO::~ImageIO()
{
	if (_rgbadData != NULL)
	{
		free( _rgbadData );
	}	
}

void ImageIO::setSize(int width, int height)
{
	if (width > 0 || height > 0)
	{
		_width = width;
		_height = height;
		if ( _rgbadData == NULL )
		{
			_rgbadData = (float *) malloc ( 5 * width * height * sizeof(float));
		} else {
			if ( sizeof ( _rgbadData ) < 5 * width * height * sizeof(float) )
			{
				free (_rgbadData);
				_rgbadData = (float *) malloc ( 5 * width * height * sizeof(float));
			}
		}
	} else {
		cout << "setSize for image must be > 0" << endl;
	}
}

void ImageIO::setRGBA(int x, int y, float r, float g, float b, float a, float depth)
{
	int index = x + y * _width;
	index *= 5;
	_rgbadData[index] = r;
	_rgbadData[index+1] = g;
	_rgbadData[index+2] = b;
	_rgbadData[index+3] = a;
	_rgbadData[index+4] = depth;
}

void ImageIO::setRGBA(int x, int y, float *rgba)
{
	int index = x + y * _width;
	index *= 5;
	for ( int i = 0; i <= 5; i++)
		_rgbadData[index+i] = rgba[i];
}

void ImageIO::getRGBA(int x, int y, float &r, float &g, float &b, float &a, float &depth)
{
	int index = x + y * _width;
	index *= 5;
	r = _rgbadData[index];
	g = _rgbadData[index+1];
	b = _rgbadData[index+2];
	a = _rgbadData[index+3];
	depth = _rgbadData[index+4];
}

void ImageIO::getRGBA(int x, int y, float *rgba)
{
	int index = x + y * _width;
	index *= 5;
	for ( int i = 0; i <= 5; i++)
		 rgba[i] = _rgbadData[index+i];
}


void ImageIO::initiateData(int width, int height, float *rgbadData)
{
	setSize(width, height);
	if (rgbadData != NULL)
	{
		memcpy(_rgbadData, rgbadData, 5*width*height*sizeof(float));
	} else {
		memset(_rgbadData, 0, 5*width*height*sizeof(float));
	}
}

void ImageIO::saveImage(const char* filename)
{
	_bmpImage.SetBitDepth(8);
	printf ( "size: %d x %d \n", _width, _height);
	_bmpImage.SetSize(_width, _height);
	RGBApixel tmp;
	for ( int i = 0; i < _height; i++)
	{
		for ( int j = 0; j < _width; j++)
		{
			int index = i*_width + j;
			index *= 5;
			tmp.Red = _rgbadData[index] * 255;
			tmp.Green = _rgbadData[index+1] * 255;
			tmp.Blue = _rgbadData[index+2] * 255;
			tmp.Alpha = 0;//(1 - _rgbadData[index+3]) * 255;

			/*tmp.Red = 0.5 * 255;
			tmp.Green = 0.5 * 255;
			tmp.Blue = 0.5 * 255;
			tmp.Alpha = 1;//(1 - _rgbadData[index+3]) * 255;*/
			_bmpImage.SetPixel(j,i,tmp);
		}
	}
	_bmpImage.WriteToFile(filename);
}

float* ImageIO::getDataPointer(int offset )
{
	float *ret;
	ret = _rgbadData;
	ret += 5 * offset;
	return ret;
}

int ImageIO::getDataLength( int size )
{
	return size*5;
}

void TransferFunction::getValue ( float val, float *rgba)
{
	vector<float>::iterator iter;
	vector<Vec4f>::iterator iterRGBA = controlRGBAList.begin();
	for ( iter = controlPointList.begin(); iter != controlPointList.end(); iter++)
	{
		if ((*iter) >= val)
			break;
		iterRGBA++;
	}
	if ( iter == controlPointList.end() )
	{
		cout << "value: " << val << " not in TransferFunction"; //<< endl;
		return;
	}
	if ( (*iter) == val ) // if we found the vale
	{
		(*iterRGBA).getValue(rgba);
		return;
	}
	if ( iter == controlPointList.begin() )
	{
		cout << "value: " << val << " not in TransferFunction" << endl;
		return;
	} else { // we did not find the value bue could interpolate it.
		float anchorR = (*iter);
		iter--;
		float anchorL = (*iter);
		float weightL = ( val - anchorL )/( anchorR - anchorL );
		float weightR = 1 - weightL;

		float rgbaL[4];
		float rgbaR[4];
		(*iterRGBA).getValue(rgbaR);
		iterRGBA--;
		(*iterRGBA).getValue(rgbaL);
		for ( int i = 0; i <= 4; i++)
		{
			rgba[i] = rgbaL[i] * weightL + rgbaR[i] * weightR;
		}
	}
}



int TransferFunction::addAnchorPoint( float val, float *rgba)
{
	vector<float>::iterator iter = controlPointList.begin();
	vector<Vec4f>::iterator iterRGBA = controlRGBAList.begin();
	if ( controlPointList.size() == 0)
	{
		controlPointList.insert(iter, val);
		Vec4f vec4;
		vec4.setValue(rgba);
		controlRGBAList.insert(iterRGBA, vec4);
		return 1;
	}

	for ( iter = controlPointList.begin(); iter != controlPointList.end(); iter++)
	{
		if ((*iter) >= val)
			break;
		iterRGBA++;
	}
	if ( (*iter) == val ) // if we found the vale, change it.
	{
		(*iterRGBA).setValue(rgba);
	} else { // if it's not there. we just insert it
		controlPointList.insert(iter, val);
		Vec4f vec4;
		vec4.setValue(rgba);
		controlRGBAList.insert(iterRGBA, vec4);
	}
	return controlPointList.size(); // return the new size.
}

void TransferFunction::resetList()
{
	controlRGBAList.empty();
	controlPointList.empty();
}

void TransferFunction::print()
{
	for ( int i = 0; i <= controlPointList.size(); i++)
	{
		float val = controlPointList[i];
		float rgba[4];
		controlRGBAList[i].getValue(rgba);
		printf ("value: %f, r: %f, g: %f, b: %f, a: %f\n", val, rgba[0], rgba[1], rgba[2], rgba[3] );
	}

}

Renderer::Renderer()
{
	_volumeData = NULL;
	
	_imgWidth = 0;
	_imgHeight = 0;
	_globalSize.setValue(0, 0, 0);
	
	_rayType = UNDEFINED;
}

Renderer::~Renderer()
{

}

void Renderer::addTransferFunction(float val, float r, float g, float b, float a)
{
	float rgba[4];
	rgba[0] = r;
	rgba[1] = g;
	rgba[2] = b;
	rgba[3] = a;
	_transferFn.addAnchorPoint(val, rgba);
}

void Renderer::setVolume(Volume *volume)
{
	_volumeData = volume;
}

void Renderer::setGlobalOffset ( int x, int y, int z)
{
	_globalOffset.setValue(x, y, z);
}

void Renderer::setGlobalSize(int x, int y, int z)
{
	_globalSize.setValue(x, y, z);	
}

void Renderer::setGlobalMinMax( float min, float max)
{
	_globalMin = min;
	_globalMax = max;
}

void Renderer::setOrthogonalDirection( int dim, bool dir, int min, int max )
{
	_dim = dim;
	_direction = dir;
	_minDim = min;
	_maxDim = max;
	_rayType = ORTHOGONAL;
}

void Renderer::updateMetaInfo()
{
	switch(_rayType)
	{
	case ORTHOGONAL:
		if (_globalSize._x == 0 || _globalSize._y == 0 || _globalSize._z == 0 )
		{
			cout << "no global size yet" << endl;
			return;
		}
		if ( _volumeData == NULL )
		{
			cout << "no volumes" << endl;
			return;
		}
		if ( _globalMin == 0 || _globalMax == 0)
		{
			cout << "no global min/max yet" << endl;
			return;
		}
		normalizeVolume();
		break;
	case UNDEFINED:
	default:
		cout << "undefined rendering scheme" << endl;
		break;
	}
	
}

void Renderer::normalizeVolume()
{
	_volumeData->normalize(_globalMin, _globalMax);
	//_volumeData->updateMetaInfo();
}

void Renderer::updateImageSize()
{
	int width, height;
	switch ( _dim )
	{
	case 0:
		width = _globalSize._y;
		height = _globalSize._z;
		break;
	case 1:
		width = _globalSize._x;
		height = _globalSize._z;
		break;
	case 2:
		width = _globalSize._x;
		height = _globalSize._y;
		break;
	}
	_image.initiateData(width, height, NULL);
}

void Renderer::render()
{
	switch(_rayType)
	{
	case ORTHOGONAL:
		int width;
		int height;
		int coord[2];
		
		switch ( _dim )
		{
		case 0:
			coord[0] = 1;
			coord[1] = 2;
			width = _globalSize._y;
			height = _globalSize._z;
			break;
		case 1:
			coord[0] = 0;
			coord[1] = 2;
			width = _globalSize._x;
			height = _globalSize._z;
			break;
		case 2:
			coord[0] = 0;
			coord[1] = 1;
			width = _globalSize._x;
			height = _globalSize._y;
			break;
		}

		int volumeSize[3];
		_volumeData->getSize(volumeSize);
		
		int frontPage, endPage; // local range of visible volumes
		frontPage = _minDim - _globalOffset[_dim];
		endPage = _maxDim -  _globalOffset[_dim];
		
/*		printf ("minDim: %d, maxDim: %d, offset: %d %d %d, size: %d %d %d, targetDim: %d, start: %d, end: %d\n",
						_minDim, _maxDim, _globalOffset[0], _globalOffset[1], _globalOffset[2],
						volumeSize[0], volumeSize[1], volumeSize[2], _dim, frontPage, endPage
				);*/
		//_image.setSize(width, height);
		_image.initiateData(width, height, NULL);

		if ( frontPage >= volumeSize[_dim] || endPage <= 0) // current node does not contain any visible volume
		{
			cout << "no data for rendering" << endl;
		} else { // visible volume
			int seq[3];
			frontPage = frontPage < 0 ? 0 : frontPage;
			endPage = endPage > volumeSize[_dim] ? volumeSize[_dim] : endPage;
			if ( _direction == true )
			{
				seq[0] = frontPage;
				seq[1] = endPage-1;
				seq[2] = 1;
			} else {
				seq[0] = endPage-1;
				seq[1] = frontPage;
				seq[2] = -1;
			}
			int pos[3];

			printf ("minDim: %d, maxDim: %d, offset: %d %d %d, size: %d %d %d, targetDim: %d, start: %d, end: %d, start: %d, end: %d\n",
							_minDim, _maxDim, _globalOffset[0], _globalOffset[1], _globalOffset[2],
							volumeSize[0], volumeSize[1], volumeSize[2], _dim, frontPage, endPage,
							seq[0], seq[1]
					);


			for ( int y = 0; y < volumeSize[coord[1]]; y++)
			{
				pos[coord[1]] = y;
				int imageY = y+_globalOffset[coord[1]];
				for ( int x = 0; x < volumeSize[coord[0]]; x++)
				{
					pos[coord[0]] = x;
					int imageX = x+_globalOffset[coord[0]];
					for ( int i = seq[0]; i != seq[1]; i += seq[2])
					{
						pos[_dim] = i;
						float val = _volumeData->getVolume(pos);
						float rgba[5];
						float rgbaX[4];
						_image.getRGBA ( imageX, imageY, rgba);
						_transferFn.getValue(val, rgbaX);
						rgba[0] = rgba[0] + rgbaX[0] * rgbaX[3] * ( 1- rgba[3]);
						rgba[1] = rgba[1] + rgbaX[1] * rgbaX[3] * ( 1- rgba[3]);
						rgba[2] = rgba[2] + rgbaX[2] * rgbaX[3] * ( 1- rgba[3]);
						rgba[3] = rgba[3] + rgbaX[3] * ( 1 - rgba[3]);
						rgba[4] = i + _globalOffset[_dim];
						//rgba[4] = _globalOffset[_dim];
						_image.setRGBA( imageX, imageY, rgba);
						if (rgba[3] > 0.97)
							break;
					}
				}
			}
		}
		break;
	case UNDEFINED:
	default:
		cout << "undefined rendering scheme" << endl;
		break;
	}
	
}

void Renderer::saveImage(const char* filename)
{
	_image.saveImage(filename);
}

ImageIO* Renderer::getImage()
{
	return &_image;
}
