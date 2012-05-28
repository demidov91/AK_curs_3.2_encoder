#pragma once
#include <vector>
#include <Windows.h>
#include <boost\filesystem.hpp>
#include "tests.h"
using namespace std;
using namespace boost ::filesystem;

#define MAX_FILE_COUNT 256
#define MAX_BLOCK_SIZE 32
#define MAX_THREAD_SIZE MAX_FILE_COUNT*MAX_BLOCK_SIZE

class EncodedDataAccessor
{
private:
	int fileCount;
	vector<const path> keys;
	vector<const string> files;
	vector<unsigned long int> availableBytes;
	vector<HANDLE> inPipes;
	vector<HANDLE> outPipes;
	void* tempBuffer;
	
public:	
	
	EncodedDataAccessor();
	/**
	Names of keys and data files.
	*/
	EncodedDataAccessor(vector<const path>* keys, vector<const string>* threads, vector<unsigned long int>* available);

	/**
	Should be invoked before getData(...);
	*/
	void Start();

	/**
	pointers - 'fileCount' pairs where: 
		[2i] - number of file
		[2i+1] - count of blocks to be read
	*/
	void getData(char* buffer, char* pointers, char blockSize);

	/**
	Alternative to Start() for testing.
	*/
	void __testStart();

	~EncodedDataAccessor(void);

	friend Friendly;
};

