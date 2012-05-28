#pragma once

#include "CommunicationalStructures.h"
#include <string>

class LengthProducer
{
private:
	int numberOfFiles;
	unsigned long int *bytesAvailable;
	int blockSize;
	bool noData;

	unsigned long int GetFSObjectSize(string);
public:
	LengthProducer(vector<const string>*, int blockSize);
	Lengthes getNextLengthes();
	~LengthProducer(void);


	/**
	For tests only.
	*/
	LengthProducer()
	{
		bytesAvailable = new unsigned long int[0];
	}

	/**
	For tests only.
	*/
	unsigned long int __GetFSObjectSize(string inp)
	{
		return GetFSObjectSize(inp);
	};
};

