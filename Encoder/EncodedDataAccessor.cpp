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
	threadsStarted = false;
};


EncodedDataAccessor* EncodedDataAccessor::create(vector<const path>* keys, vector<const string>* threads)
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
	initCrSection();
	threadsStarted = true;
	for(int i = 0; i < fileCount; i++)
	{
		CreatePipe(&outPipes[i], &inPipes[i], NULL, MAX_THREAD_SIZE);
		ArgsForAsyncEncoder* argsForEncoding = (ArgsForAsyncEncoder*)malloc(sizeof(ArgsForAsyncEncoder));
		argsForEncoding ->key = keys[i];
		argsForEncoding ->file = files[i];
		argsForEncoding ->pipe = &inPipes[i];
		argsForEncoding ->byteToTalk = &talkBytes[i];
		args[i] = argsForEncoding;
		_beginthread(encode_async, 100, args[i]);
	}
}

void EncodedDataAccessor ::getData(BYTE* buffer, BYTE* pointers, char blockSize)
{
	for(int i = 0; i < fileCount; i++)
	{
		char fileNumber = i;
		int lengthToRead = pointers[2*i+1] * blockSize;
		int remainsToRead = lengthToRead;
		DWORD wasAvailable = 0;
		DWORD canRead = 0;
		long int win = 0;
		bool threadWorkedBeforeAnswer = talkBytes[i];
		while(remainsToRead > 0)
		{			
			if (wasAvailable == 0)
			{
				threadWorkedBeforeAnswer = talkBytes[i] != 0;
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
				if(!threadWorkedBeforeAnswer)
				{
					break;
				}
				else
				{
					Sleep(1000);
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
		FILE* file = fopen(files[i], "rb");
		char buffer[102];
		fread(buffer, 1, 100, file);
		DWORD temp;
		WriteFile(inPipes[i], buffer, 100, &temp, NULL);		
		fclose(file);
		talkBytes[i] = 0;
		ArgsForAsyncEncoder* argsForEncoding = (ArgsForAsyncEncoder*)malloc(sizeof(ArgsForAsyncEncoder));
		argsForEncoding ->key = keys[i];
		argsForEncoding ->file = files[i];
		argsForEncoding ->pipe = &inPipes[i];
		argsForEncoding ->byteToTalk = &talkBytes[i];
		args[i] = argsForEncoding;
	}
}



EncodedDataAccessor::~EncodedDataAccessor(void)
{	
	if(tempBuffer)
	{
		free(tempBuffer);
	}
	for(int i = 0; i < fileCount; i++)
	{
		free(keys[i]);
		free(args[i]);
	}
	if(threadsStarted)
	{
		destroyCrSection();
	}
}
