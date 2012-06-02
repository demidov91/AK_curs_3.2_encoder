#include "RandomGenerator.h"
#include <algorithm>
#include <cstdlib>
#include "Constants.h"

RandomGenerator ::RandomGenerator()
{
	map = new BYTE[0];
	copyBuffer = new BYTE[0];
};


RandomGenerator* RandomGenerator ::create(unsigned char blockSize)
{
	this ->blockSize = blockSize;
	map = new BYTE[blockSize*BLOCK_COUNT];
	copyBuffer = new BYTE[blockSize*BLOCK_COUNT];
	for(int i = 0; i < BLOCK_COUNT; i++) map[i]=i;
	genNextRandom();
	return this;
};


void RandomGenerator ::getNewKey(char* buffer)
{
	for(int i = 0; i < 112; i++)
	{
		buffer[i] = rand();
	}
}





void RandomGenerator ::genNextRandom()
{
	random_shuffle(map, map+BLOCK_COUNT);
}

unsigned char* RandomGenerator ::getMapPointer()
{
	return map;
}


void RandomGenerator ::shuffle(BYTE* buffer)
{
	for(int i = 0; i < BLOCK_COUNT; i++)
	{
		memcpy(&copyBuffer[map[i]*blockSize], &buffer[i*blockSize], blockSize);
	}
	memcpy(buffer, copyBuffer, blockSize*BLOCK_COUNT);
}


RandomGenerator::~RandomGenerator()
{
	free(map);
	delete[] copyBuffer;
}