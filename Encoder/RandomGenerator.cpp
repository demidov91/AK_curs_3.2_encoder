#include "RandomGenerator.h"
#include <algorithm>
#include <cstdlib>

RandomGenerator ::RandomGenerator()
{};


RandomGenerator ::RandomGenerator(unsigned char blockSize)
{
	this ->blockSize = blockSize;
};


void RandomGenerator ::getNewKey(char* buffer)
{
	for(int i = 0; i < 112; i++)
	{
		buffer[i] = rand();
	}
}



vector<int> RandomGenerator ::randomVector(int n)
{
	vector<int> toReturn;	
	for (int i=0; i<n; ++i) toReturn.push_back(i);
	random_shuffle ( toReturn.begin(), toReturn.end() );
	return toReturn;
}
