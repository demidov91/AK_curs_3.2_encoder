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


class ArgsForAsyncEncoder
{
public:
	const char* key;
	const char* file;
	PHANDLE pipe;
	char* byteToTalk;
};