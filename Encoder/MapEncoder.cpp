#include "MapEncoder.h"
#include <vector>
#include <string>
#include "Constants.h"






MapEncoder::MapEncoder()
{

}

MapEncoder* MapEncoder::create(vector<const string>* keys)
{
	for(vector<const string> ::iterator it = keys ->begin(); it != keys ->end(); it++)
	{
		ByteEncoder* thread = new ByteEncoder(&*it);
		threads.push_back(thread);
	}
	threadCount = keys ->size();
	return this;
}

void MapEncoder::encodePointersAndMap(char* pointers, char* map)
{
	for(int i = 0; i < threadCount; i++)
	{
		BYTE start = pointers[2*i];
		threads[i] ->encodeByte(pointers+2*i);
		BYTE length = pointers[2*i+1];
		threads[i] ->encodeByte(pointers+2*i+1);
		for(int j = start; j < start+length; j++)
		{
			threads[i] ->encodeByte(map + j);
		}	
	}
}

MapEncoder::~MapEncoder()
{
	for(vector<ByteEncoder*> ::iterator it = threads.begin(); it != threads.end(); it++)
	{
		delete *it;
	}	

}