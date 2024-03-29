#pragma once

#include <vector>
#include <boost\filesystem.hpp>
#include "tests.h"
#include "EncodedDataAccessor.h"
#include "LengthProducer.h"
#include "RandomGenerator.h"
#include "MapEncoder.h"
using namespace std;
using namespace boost ::filesystem;


class Encoder
{
private:
	int data_block;
	vector<const string> targets;
	list<path> availableKeys;
	vector<const path> keys;
	vector<const string> verbalKeys;
	LengthProducer lengther;
	EncodedDataAccessor dataAccessor;
	RandomGenerator randomer;
	MapEncoder mapper;
	FILE* outputFile;

	/**
	1 + 512 + 256 + 256*x
	----------------------   ==  p
			256*x

	Returns solvation of equation if 100 < percentage < 400 otherwise 0 
	*/
	int countDataBlock(int percentage);
	path keyForThread(const string*);
	void formCollectionOfAvailableKeys(const char*);
	path generateKeyForThread(const string*, unsigned char);
	path renameKey(path* oldPath, const string* ,unsigned char thread);
	/**
	Run after lengther, dataAccessor, randomer, mapper and outputFile are created and configured. Arguement - number of threads.
	*/
	void runEncodingCycle(BYTE);
	void tellPercentageDone(float);
	class EncoderInteractive
	{
	private:
		bool filenameForAll;
		bool previostForAll;
		string previostAnswer;
	public:
		EncoderInteractive();
		string askForVerbalKey(const string target, const string proposal);
	
	};
	

public:
	Encoder();

	/**
	Enodes all files in working directory except files in 'except' vector or only in 'only' vector.
	Parameter size supposed to be used like a percentage of value of output file in comparison to input sum file values.
	Returns number of encoded files.
	*/
	int start(vector<const string>* except, const  vector<const string>* only, int size);

	int start(vector<const string>* except, const  vector<const string>* only);

	~Encoder();


	/**
	For tests only.
	*/
	int __CountDataBlock(int percentage)
	{
		return countDataBlock(percentage);
	};

	/**
	For teste only.
	*/
	friend Friendly;
	

};