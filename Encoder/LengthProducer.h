#pragma once

#include "CommunicationalStructures.h"
#include <string>
#include "tests.h"
#include "Constants.h"
using namespace std;


class LengthProducer
{
private:
	int numberOfFiles;
	vector<unsigned long int> bytesAvailable;
	int blockSize;
	bool noData;
	unsigned long int initialBlocksCount;
public:
	LengthProducer();
	LengthProducer* create(vector<const string>*, int blockSize);
	Lengthes getNextLengthes();
	float getProcessedPart();

	friend Friendly;
};

