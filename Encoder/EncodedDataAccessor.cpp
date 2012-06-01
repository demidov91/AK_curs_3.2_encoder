#include "EncodedDataAccessor.h"
#include <string.h>
#include <Windows.h>
#include "EnDecoding.h"
#include "logging.h"
#include <algorithm>
#include <process.h>
#include "CommunicationalStructures.h"


EncodedDataAccessor::EncodedDataAccessor()
{
	tempBuffer = malloc(0);
};


EncodedDataAccessor* EncodedDataAccessor::create(vector<const path>* keys, vector<const string>* threads, vector<unsigned long int>* availableBytes)
{
	this ->fileCount = threads ->size();
	this ->keys.resize(fileCount, NULL);
	for(int i = 0; i < fileCount; i++)
	{
		string key = (*keys)[i].string();
		this ->keys[i] = (char*)malloc(key.length() + 1);
		strcpy(this ->keys[i], key.c_str());
		this ->files.push_back((*threads)[i].c_str());
	}
	this ->inPipes.resize(fileCount, NULL);
	this ->outPipes.resize(fileCount, NULL);
	memset(talkBytes, 1, fileCount);
	tempBuffer = malloc(MAX_FILE_COUNT * MAX_BLOCK_SIZE);
	return this;
}

void EncodedDataAccessor ::Start()
{
	for(int i = 0; i < fileCount; i++)
	{
		CreatePipe(&outPipes[i], &inPipes[i], NULL, MAX_THREAD_SIZE);
		ArgsForAsyncEncoder* argsForEncoding = new ArgsForAsyncEncoder();
		argsForEncoding ->key = keys[i];
		argsForEncoding ->file = files[i];
		argsForEncoding ->pipe = &inPipes[i];
		argsForEncoding ->byteToTalk = &talkBytes[i];
		args.push_back(argsForEncoding);
		_beginthread(encode_async, 100, args.back());
	}
}

void EncodedDataAccessor ::getData(char* buffer, char* pointers, char blockSize)
{
	for(int i = 0; i < fileCount; i++)
	{
		char fileNumber = pointers[2*i];
		int lengthToRead = pointers[2*i+1] * blockSize;
		int remainsToRead = lengthToRead;
		DWORD wasAvailable = 0;
		DWORD canRead = 0;
		while(remainsToRead > 0)
		{
			if (wasAvailable <= 0)
			{
				PeekNamedPipe(outPipes[fileNumber], tempBuffer, remainsToRead, &canRead, &wasAvailable, NULL);
			}
			else 
			{
				canRead = min(wasAvailable, remainsToRead);
			}
			if(canRead)
			{
				ReadFile(outPipes[fileNumber], buffer, canRead, &canRead, NULL);
				wasAvailable -= canRead;
				remainsToRead -= canRead;
				buffer += canRead;
			}
			else
			{
				if(!talkBytes[i])
				{
					break;
				}
			}
			
		}
		for(int i = 0; i < remainsToRead; i++)
		{
			*buffer = 0;
			buffer++;
		}

	}
}

void EncodedDataAccessor ::__testStart()
{
	for(int i = 0; i < fileCount; i++)
	{
		CreatePipe(&outPipes[i], &inPipes[i], NULL, 200);
		void* argsForEncoding[3];
		FILE* file = fopen(files[i], "rb");
		char buffer[102];
		fread(buffer, 1, 100, file);
		DWORD temp;
		WriteFile(inPipes[i], buffer, 100, &temp, NULL);		
		fclose(file);
		talkBytes[i] = 0;
	}
}

bool EncodedDataAccessor::getErrors()
{
	return noDataError;
}

EncodedDataAccessor::~EncodedDataAccessor(void)
{	
	if(tempBuffer)
	{
		free(tempBuffer);
	}
	for(vector<ArgsForAsyncEncoder*> ::iterator it = args.begin(); it != args.end(); it++)
	{
		calloc(strlen((*it) ->key), 1);
		delete *it;
	}
	
}
