#include "LengthProducer.h"
#include <stdio.h>
#include <algorithm>
#include <math.h>
#include <limits.h>
#include "logging.h"
#include <boost\filesystem.hpp>
#include "FilesystemHelper.h"
using namespace boost ::filesystem;


LengthProducer::LengthProducer()
{}

LengthProducer* LengthProducer::create(vector<const string>* fileNames, int blockSize)
{
	this ->numberOfFiles = fileNames ->size();
	for(vector<const string> ::iterator fsObj = fileNames ->begin(); fsObj < fileNames ->end();fsObj++)
	{
		unsigned long int fsObjectSize = GetFSObjectSize(&*fsObj);
		unsigned long int blocksInObject = fsObjectSize / blockSize;
		if (fsObjectSize % blockSize)
		{
			blocksInObject++;			
		}
		bytesAvailable.push_back(blocksInObject);
	}
	this ->noData = false;
	return this;
}

Lengthes LengthProducer ::getNextLengthes()
{
	if(noData)
	{
		Lengthes noDataResult;
		noDataResult.answer = vector<ThreadLengthPair>();
		noDataResult.count = 0;
		return noDataResult;
	}
	unsigned long int minFile = *min_element(bytesAvailable.begin(), bytesAvailable.end());
	unsigned long int* tails = new unsigned long int [numberOfFiles];
	unsigned long int totalTail = 0;
	for(int i = 0; i < numberOfFiles; i++)
	{
		tails[i] = bytesAvailable[i] - minFile;
		totalTail += tails[i];
	}
	int* sizes = new int[numberOfFiles];
	long int steps;
	if((totalTail + minFile*numberOfFiles) < BLOCK_COUNT)
	{
		for(int i = 0; i < numberOfFiles; i++)
		{
			sizes[i] = bytesAvailable[i];						
		}
		steps = 1;
	}
	else
	{
		if(totalTail < BLOCK_COUNT)
		{
			totalTail = 0;
			for(int i = 0; i < numberOfFiles; i++)
			{
				tails[i] = bytesAvailable[i];
				totalTail += tails[i];
			}			
		}
		int blocksAvailable = BLOCK_COUNT;
		long int* stepsNecessary = new long int[numberOfFiles];
		for(int i = 0; i <numberOfFiles; i++)
		{
			sizes[i] = unsigned long int(floor((float)blocksAvailable / totalTail * tails[i]));
			blocksAvailable -= sizes[i];
			totalTail -= tails[i];
			stepsNecessary[i] = sizes[i] == 0? LONG_MAX : long int(tails[i] / sizes[i]);
		}
		steps = *min_element(stepsNecessary, &stepsNecessary[numberOfFiles]);
		if(steps == 0)
		{
			log_error("NumberOfSteps is zero");
		}
		delete[] stepsNecessary;
	}
	noData = false;
	for(int i = 0; i < numberOfFiles; i++)
	{
		bytesAvailable[i] -= steps*sizes[i];
		noData |= bytesAvailable[i];
	}
	noData = !noData;
	vector<ThreadLengthPair> map;
	for(int i = 0; i <numberOfFiles; i++)
	{
		map.push_back(ThreadLengthPair(i, sizes[i]));
	}
	Lengthes toReturn;
	toReturn.count = steps;
	toReturn.answer = map;	
	delete[] sizes;
	delete[] tails;
	return toReturn;
}




vector<unsigned long int> LengthProducer ::getAllAvailable()
{
	return bytesAvailable;
};



