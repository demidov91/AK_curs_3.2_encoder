#pragma once
#include <vector>
#include <Windows.h>
using namespace std;

struct ThreadLengthPair
{
	ThreadLengthPair(int, int);
	int thread;
	int length;
};


struct Lengthes
{
	int count;	
	vector<ThreadLengthPair> answer;
};


struct ArgsForAsyncEncoder
{

	const char* key;
	const char* file;
	PHANDLE pipe;
	char* byteToTalk;
};