#pragma once

#include <vector>
#include <string>
#include "tests.h"
#include "Constants.h"
#include "ByteEncoder.h"
using namespace std;

class MapEncoder
{
private:
	vector<ByteEncoder*> threads;
	int threadCount;
public:
	MapEncoder();
	MapEncoder* create(vector<const string>*);
	void encodePointersAndMap(char*, char*);
	~MapEncoder();
friend Friendly;
};

