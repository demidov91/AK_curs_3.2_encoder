#pragma once

#include "CommunicationalStructures.h"

class LengthProducer
{
private:
	int numberOfFiles;
	unsigned long int *bytesAvailable;
	int blockSize;
	bool noData = false;
public:
	LengthProducer(int numberOfFiles, const char** fileNames, int blockSize);
	Lengthes getNextLengthes();
	~LengthProducer(void);
};

