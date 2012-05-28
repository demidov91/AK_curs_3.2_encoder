#include "EncodedDataAccessor.h"
#include <string.h>
#include <Windows.h>
#include "EnDecoding.h"
#include "logging.h"
#include <algorithm>


EncodedDataAccessor::EncodedDataAccessor()
{
	tempBuffer = malloc(0);
};


EncodedDataAccessor::EncodedDataAccessor(vector<const path>* keys, vector<const string>* threads, vector<unsigned long int>* availableBytes)
{
	this ->fileCount = threads ->size();
	this ->keys = *keys;
	this ->files = *threads;
	this ->inPipes.resize(fileCount, NULL);
	this ->outPipes.resize(fileCount, NULL);
	this ->availableBytes = *availableBytes;
	tempBuffer = malloc(MAX_FILE_COUNT * MAX_BLOCK_SIZE);
}

void EncodedDataAccessor ::Start()
{
	for(int i = 0; i < fileCount; i++)
	{
		CreatePipe(&outPipes[i], &inPipes[i], NULL, MAX_THREAD_SIZE);
		void* argsForEncoding[3];
		argsForEncoding[0] = &keys[i];
		argsForEncoding[1] = &files[i];
		argsForEncoding[2] = &inPipes[i];
		encode_async(argsForEncoding);
	}
}

void EncodedDataAccessor ::getData(char* buffer, char* pointers, char blockSize)
{
	for(int i = 0; i < fileCount; i++)
	{
		char fileNumber = pointers[2*i];
		int lengthToRead = pointers[2*i+1] * blockSize;
		int remainsToRead = min(lengthToRead, availableBytes[i]);
		int wasRead = remainsToRead;
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
			ReadFile(outPipes[fileNumber], buffer, canRead, &canRead, NULL);
			wasAvailable -= canRead;
			remainsToRead -= canRead;
			buffer += canRead;
		}
		availableBytes[i] -= wasRead;
		for(int i = 0; i < lengthToRead - wasRead; i++)
		{
			buffer[0] = 0;
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
		FILE* file = fopen(files[i].c_str(), "rb");
		char buffer[102];
		fread(buffer, 1, 100, file);
		DWORD temp;
		WriteFile(inPipes[i], buffer, 100, &temp, NULL);		
		fclose(file);
	}

}

EncodedDataAccessor::~EncodedDataAccessor(void)
{	
	if(tempBuffer)
	{
		free(tempBuffer);
	}
	
}
