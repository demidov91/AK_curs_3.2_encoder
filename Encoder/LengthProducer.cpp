#include "LengthProducer.h"
#include <stdio.h>
#include <algorithm>
#include <math.h>
#include <limits.h>
#include "logging.h"
#include <boost\filesystem.hpp>
using namespace boost ::filesystem;

LengthProducer::LengthProducer(vector<const string>* fileNames, int blockSize)
{
	this ->numberOfFiles = fileNames ->size();
	bytesAvailable = new unsigned long int[numberOfFiles];
	
	int threadIterator = 0;
	for(vector<const string> ::iterator fsObj = fileNames ->begin(); fsObj < fileNames ->end();fsObj++)
	{
		bytesAvailable[threadIterator] = GetFSObjectSize(*fsObj);		
		threadIterator++;
	}
	this ->blockSize = blockSize;
	this ->noData = false;
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
	unsigned long int minFile = *min_element(bytesAvailable, &bytesAvailable[numberOfFiles]);
	unsigned long int* tails = new unsigned long int [numberOfFiles];
	unsigned long int totalTail = 0;
	for(int i = 0; i < numberOfFiles; i++)
	{
		tails[i] = bytesAvailable[i] - minFile;
		totalTail += tails[i];
	}
	int* sizes = new int[numberOfFiles];
	long int steps;
	if((totalTail < blockSize) && ((totalTail + minFile*numberOfFiles) < blockSize))
	{
		for(int i = 0; i < numberOfFiles; i++)
		{
			sizes[i] = bytesAvailable[i];						
		}
		steps = 1;
	}
	else
	{
		if(totalTail < blockSize)
		{
			unsigned long int additionalBlock = (blockSize - totalTail) / numberOfFiles;
			if ((blockSize - totalTail) % numberOfFiles > 0)
			{
				additionalBlock++;
			}
			for(int i = 0; i < numberOfFiles; i++)
			{
				tails[i] += additionalBlock;
			}
			totalTail += additionalBlock*numberOfFiles;
		}
		int blockAvailable = blockSize;
		long int* stepsNecessary = new long int[numberOfFiles];
		for(int i = 0; i <numberOfFiles; i++)
		{
			sizes[i] = unsigned long int(floor((float)blockAvailable / totalTail * tails[i]));
			blockAvailable -= sizes[i];
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

unsigned long int LengthProducer ::GetFSObjectSize(string root)
{
	if (!is_directory(root))
	{
		return file_size(root);
	}
	recursive_directory_iterator end;
	unsigned long int answer = 0;
	for(recursive_directory_iterator it = recursive_directory_iterator(root); it != end; it++)
	{
		if(!is_directory(*it))
		{
			answer += file_size(*it);
		}
	}
	return answer;
};



LengthProducer::~LengthProducer(void)
{
	delete[] bytesAvailable;
}
