#pragma once

#include <vector>
#include <string>
#include "tests.h"
#include "Constants.h"
using namespace std;

class MapEncoder
{
private:
	class ThreadMapEncoder
	{
	private:
		char* key;
		int currentPosition;
		int length;
	public:
		ThreadMapEncoder(string*);
		void encodeByte(char*);
		~ThreadMapEncoder();
	friend Friendly;
	};	
	vector<ThreadMapEncoder*> threads;
	int threadCount;
public:
	MapEncoder();
	MapEncoder* create(vector<const string>*);
	void encodePointersAndMap(char*, char*);
	~MapEncoder();
friend Friendly;
};

