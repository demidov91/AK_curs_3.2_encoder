#pragma once

#include "CommunicationalStructures.h"
#include <string>
#include "tests.h"
#define BLOCK_COUNT 256
using namespace std;


class LengthProducer
{
private:
	int numberOfFiles;
	vector<unsigned long int> bytesAvailable;
	int blockSize;
	bool noData;

	unsigned long int GetFSObjectSize(string);
public:
	LengthProducer(vector<const string>*, int blockSize);
	Lengthes getNextLengthes();
	vector<unsigned long int> getAllAvailable();
	


	/**
	For tests only.
	*/
	LengthProducer()
	{
		
	}

	/**
	For tests only.
	*/
	unsigned long int __GetFSObjectSize(string inp)
	{
		return GetFSObjectSize(inp);
	};

	friend Friendly;
};

