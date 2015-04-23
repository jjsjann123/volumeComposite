/*!
 *
 *	CS 595 Assignment 10
 *	Jie Jiang
 *	
 */
#include <mpi.h>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <fstream>
#include <cmath>
#include <vector>


#include "volume.h"
#include "renderer.h"
#include "compositor.h"

using AJParallelRendering::Volume;
using AJParallelRendering::Renderer;
using AJParallelRendering::Compositor;

using namespace std;

struct Vec3f{
	float x;
	float y;
	float z;
};

struct Vec3i{
	int x;
	int y;
	int z;
};

struct ChunkData{
	Vec3i offset;
	Vec3i range;
};

int main(int argc, char** argv)
{
	int myrank;
	int npes;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &npes);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	int xPartition, yPartition, zPartition;
	char *filename;
	
	if (argc != 10)
	{
		if ( myrank == 0 )
		{
			cout << "arguments not match, error..." << endl;
			cout << "usage: <program> <data file>  <xpartitions> <ypartitions> <zpartitions> [dim] [dir] [min] [max] [val]" << endl;
		}
		return 1;
	}
	filename = argv[1];
	xPartition = atoi(argv[2]);
	yPartition = atoi(argv[3]);
	zPartition = atoi(argv[4]);
	int targetDim = atoi(argv[5]);
	int dir = atoi(argv[6]);
	int min = atoi(argv[7]);
	int max = atoi(argv[8]);
	float val = atof(argv[9]);

	int d = 0;
	for ( int i = 0; pow(2, i) <= npes - 1; i++)
	{
		if ( pow(2, i) == npes - 1)
		{
			d = i;
			break;
		}
	}
	if ( d == 0 )
	{
		if ( myrank == 0)
		{
			cout << "npes number of proper. Use a npes = pow(2,i)+1" << endl;
		}
		return 1;
	} else {
		if ( myrank == 0)
		{
			cout << "**************:   " << d << endl;
		}
	}


	int decomDim = 0;
	if (xPartition == npes-1)
	{
		decomDim = 0;
	} else if ( yPartition == npes-1 )
	{
		decomDim = 1;
	} else if ( zPartition == npes-1 )
	{
		decomDim = 2;
	} else
	{
		if ( myrank == 0 )
		{
			cout << "one of <xpartitions> <ypartitions> <zpartitions> has to be npes-1" << endl;
		}
		return 1;
	}

	int dim[3];
	ifstream input (filename, ios::binary);
	input.read( (char*) &dim[0], sizeof(int));
	input.read( (char*) &dim[1], sizeof(int));
	input.read( (char*) &dim[2], sizeof(int));

	float *buffer;
	int *chunkListBuffer;
	int chunkBuffer[6];

	// global min/max
	float minMax[2];

	if ( myrank == 0)
	{
		// Load the data and get dimension out
		ifstream input (filename, ios::binary);
		input.read( (char*) &dim[0], sizeof(int));
		input.read( (char*) &dim[1], sizeof(int));
		input.read( (char*) &dim[2], sizeof(int));
		MPI_Bcast(dim, 3, MPI_INT, myrank, MPI_COMM_WORLD);

		Vec3i chunkSize;
		Vec3i chunkOffset;

		chunkListBuffer = (int *) malloc ( npes * 6 * sizeof ( int ) );

		// Calculate chunk size and range
		for ( int x = 0; x < xPartition; x++)
		{
			if ( x == xPartition - 1 )
				chunkSize.x = dim[0] % xPartition + dim[0] / xPartition;
			else
				chunkSize.x = dim[0] / xPartition;
			chunkOffset.x = x * ( dim[0] / xPartition );
			for ( int y = 0; y < yPartition; y++)
			{
				if ( y == yPartition -1 )
					chunkSize.y = dim[1] % yPartition + dim[1] / yPartition;
				else
					chunkSize.y = dim[1] / yPartition;
				chunkOffset.y = y * ( dim[1] / yPartition );
				for ( int z = 0; z < zPartition; z++)
				{
					if ( z == zPartition -1 )
						chunkSize.z = dim[2] % zPartition + dim[2] / zPartition;
					else
						chunkSize.z = dim[2] / zPartition;
					chunkOffset.z = z * ( dim[2] / zPartition );
					int index = x+xPartition*(y+z*yPartition);
					int offset = (index + 1) * 6;
					chunkListBuffer[offset] = chunkOffset.x;
					chunkListBuffer[offset+1] = chunkOffset.y;
					chunkListBuffer[offset+2] = chunkOffset.z;
					chunkListBuffer[offset+3] = chunkSize.x;
					chunkListBuffer[offset+4] = chunkSize.y;
					chunkListBuffer[offset+5] = chunkSize.z;
					printf ("Subvolume <%d, %d>, <%d, %d>, <%d, %d> is assigned to process <%d>\n",
							chunkOffset.x, chunkOffset.x + chunkSize.x - 1,
							chunkOffset.y, chunkOffset.y + chunkSize.y - 1,
							chunkOffset.z, chunkOffset.z + chunkSize.z - 1,
							index);
				}
			}
		}

		// distribute data chunk info
		MPI_Scatter(chunkListBuffer, 6, MPI_FLOAT, chunkBuffer, 6, MPI_FLOAT, 0, MPI_COMM_WORLD);

		// distribute volume data
		int bufferSize = dim[0]*dim[1];
		buffer = (float*) malloc (bufferSize*sizeof(float));
		for ( int i = 0; i <= dim[2]; i++)
		{
			input.read( (char*) buffer, dim[0]*dim[1]*sizeof(float));
			MPI_Bcast(buffer, bufferSize, MPI_INT, myrank, MPI_COMM_WORLD);
		}

		// collect local min/max and redistribute global min/max
		MPI_Gather(minMax, 2, MPI_FLOAT, buffer, 2, MPI_FLOAT, 0, MPI_COMM_WORLD);
		for (int i = 2; i <= npes; i++)
		{
			buffer[2] = buffer[2] < buffer[i*2] ? buffer[2] : buffer[i*2];
			buffer[3] = buffer[3] > buffer[i*2+1] ? buffer[3] : buffer[i*2+1];
		}
		minMax[0] = buffer[2];
		minMax[1] = buffer[3];
		printf ( "global min: %f max: %f\n", minMax[0], minMax[1]);
		MPI_Bcast(minMax, 2, MPI_FLOAT, myrank, MPI_COMM_WORLD);

		MPI_Barrier(MPI_COMM_WORLD);

		Renderer myRenderer;
		myRenderer.setGlobalSize(dim[0], dim[1], dim[2]);
		bool direction = dir > 0 ? true : false;
		myRenderer.setOrthogonalDirection( targetDim, direction, min, max );
		myRenderer.updateImageSize();

		Compositor myCompositor;
		myCompositor.setImage(myRenderer.getImage());
		myCompositor.composite(myrank, d);

		myCompositor.saveImage("composite.bmp");

		free(buffer);
		free(chunkListBuffer);
	} else { // myrank != 0
		// Receive data dimension from root
		MPI_Bcast(dim, 3, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Scatter(chunkListBuffer, 6, MPI_FLOAT, chunkBuffer, 6, MPI_FLOAT, 0, MPI_COMM_WORLD);

		//printf( "Hello world from process %d of %d: dim %d, %d, %d; chunk: %d %d %d %d %d %d\n", myrank, npes, dim[0], dim[1], dim[2], chunkBuffer[0], chunkBuffer[1], chunkBuffer[2], chunkBuffer[3], chunkBuffer[4], chunkBuffer[5]);
		int dataSize = chunkBuffer[3] * chunkBuffer[4] * chunkBuffer[5];

		int bufferSize = dim[0]*dim[1];
		buffer = (float*) malloc (bufferSize*sizeof(float));

		Volume volumeStride(chunkBuffer+3);
		for ( int i = 0; i <= dim[2]; i++)
		{
			MPI_Bcast(buffer, bufferSize, MPI_INT, 0, MPI_COMM_WORLD);
			if ( i >= chunkBuffer[2] && i < chunkBuffer[2] + chunkBuffer[5])
			{
				int posZ = i - chunkBuffer[2];
				for ( int u = chunkBuffer[0]; u < chunkBuffer[0] + chunkBuffer[3]; u++)
				{
					int posX = u - chunkBuffer[0];
					for ( int v = chunkBuffer[1]; v < chunkBuffer[1] + chunkBuffer[4]; v++)
					{
						int posY = v - chunkBuffer[1];
						int position = u + dim[0]*v;
						volumeStride.setVolume(posX, posY, posZ, buffer[position] );
					}
				}
			}
		}

		volumeStride.updateMetaInfo();
		printf( "process %d data range: <%f %f>\n", myrank, volumeStride._min, volumeStride._max);
		minMax[0] = volumeStride._min;
		minMax[1] = volumeStride._max;
		MPI_Gather(minMax, 2, MPI_FLOAT, buffer, 2, MPI_FLOAT, 0, MPI_COMM_WORLD);
		MPI_Bcast(minMax, 2, MPI_FLOAT, myrank, MPI_COMM_WORLD);

		Renderer myRenderer;
		myRenderer.setVolume(&volumeStride);
		myRenderer.setGlobalSize(dim[0], dim[1], dim[2]);
		myRenderer.setGlobalMinMax(minMax[0], minMax[1]);
		bool direction = dir > 0 ? true : false;
		myRenderer.setOrthogonalDirection( targetDim, direction, min, max );
		myRenderer.setGlobalOffset(chunkBuffer[0], chunkBuffer[1], chunkBuffer[2]);

		for ( int i = 0; i < 5; i++)
		{
			if ( i < 3)
			{
				myRenderer.addTransferFunction(0, 0, 0, 0, 0);
				myRenderer.addTransferFunction(val/2, 1, 0, 0, 0.5);
			} else if ( i == 3)
			{
				myRenderer.addTransferFunction(val, 0, 1, 1, 1);
			} else {
				myRenderer.addTransferFunction(val + (1-val)/2, 0.5, 0.2, 0.4, 0.5);
				myRenderer.addTransferFunction(1, 0, 1, 0, 0);
			}
		}

		myRenderer.updateMetaInfo();
		//printf( "process %d data range: <%f %f>\n", myrank, volumeStride._min, volumeStride._max);
		MPI_Barrier(MPI_COMM_WORLD);

		myRenderer.render();
		ostringstream convert;
		convert << myrank;
		string q = convert.str();
		q.append(".bmp");

		Compositor myCompositor;
		myCompositor.setImage(myRenderer.getImage());
		myRenderer.saveImage(q.c_str());
		myCompositor.setDirection(direction);
		myCompositor.composite(myrank, d);
		ostringstream convert2;
		convert2 << myrank;
		string q2 = convert.str();
		q2.append("_comp.bmp");
		myCompositor.saveImage(q2.c_str());

		free(buffer);

	}


	MPI_Finalize();
	return 0;
}
