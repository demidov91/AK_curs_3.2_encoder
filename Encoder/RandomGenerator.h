#pragma once
#include <vector>
#include "tests.h"
#include "Constants.h"
using namespace std;


class RandomGenerator
{
private:
	BYTE blockSize;
	BYTE* map;
	BYTE* copyBuffer;
public:
	RandomGenerator();

	/**
	This constructor specified to use R.Generator inside Encoder.
	*/
	RandomGenerator* create(BYTE blockSize);


	/**
	Writes 112 random bytes into buffer.
	*/
	void getNewKey(char* buffer);	
	
	void genNextRandom();

	BYTE* getMapPointer();

	void shuffle(BYTE*);

	~RandomGenerator();
		
	friend Friendly;
};

