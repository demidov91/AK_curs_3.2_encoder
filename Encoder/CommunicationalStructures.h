#pragma once
#include <vector>
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