#include "MapEncoder.h"
#include <vector>
#include <string>
#include "Constants.h"


MapEncoder ::ThreadMapEncoder::ThreadMapEncoder(string* key)
{
	length = key ->length();
	this ->key = (char*)malloc(length + 1);
	strcpy(this ->key, key ->c_str());
	currentPosition = 0;
}

void MapEncoder ::ThreadMapEncoder ::encodeByte(char* pointer)
{
	*pointer = *pointer ^ key[currentPosition++];
	if(currentPosition >= length)
	{
		currentPosition = 0;
	}
}


MapEncoder ::ThreadMapEncoder::~ThreadMapEncoder()
{
	free(key);
}

MapEncoder::MapEncoder()
{

}

MapEncoder* MapEncoder::create(vector<const string>* keys)
{
	for(vector<const string> ::iterator it = keys ->begin(); it != keys ->end(); it++)
	{
		ThreadMapEncoder* thread = new ThreadMapEncoder(&*it);
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
	for(vector<ThreadMapEncoder*> ::iterator it = threads.begin(); it != threads.end(); it++)
	{
		delete *it;
	}	

}