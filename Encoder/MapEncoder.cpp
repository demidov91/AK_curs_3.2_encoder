#include "MapEncoder.h"
#include <vector>
#include <string>


MapEncoder ::ThreadMapEncoder::ThreadMapEncoder(string* key)
{
	length = key ->length();
	this ->key = (char*)malloc(length + 1);
	strcpy(this ->key, key ->c_str());
	currentPosition = 0;
}

void MapEncoder ::ThreadMapEncoder ::encodeByte(char* pointer)
{}


MapEncoder ::ThreadMapEncoder::~ThreadMapEncoder()
{
	free(key);
}

MapEncoder::MapEncoder()
{}

MapEncoder::MapEncoder(vector<const string>* keys)
{
	for(vector<const string> ::iterator it = keys ->begin(); it != keys ->end(); it++)
	{
		ThreadMapEncoder* thread = new ThreadMapEncoder(&*it);
		threads.push_back(thread);
	}	
}

void MapEncoder::encodePointersAndMap(char* pointers, char* map)
{
}

MapEncoder::~MapEncoder()
{
	for(vector<ThreadMapEncoder*> ::iterator it = threads.begin(); it != threads.end(); it++)
	{
		delete *it;
	}	

}