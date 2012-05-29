#pragma once
#include <vector>
using namespace std;


class RandomGenerator
{
private:
	unsigned char blockSize;

public:
	RandomGenerator();

	/**
	This constructor specified to use R.Generator inside Encoder.
	*/
	RandomGenerator(unsigned char blockSize);


	/**
	Writes 112 random bytes into buffer.
	*/
	void getNewKey(char* buffer);	
	/**
	Generates shuffled vector of length n. 
	*/
	vector<int> randomVector(int n);
		
};

