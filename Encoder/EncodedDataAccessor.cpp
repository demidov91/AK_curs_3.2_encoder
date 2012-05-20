#include "EncodedDataAccessor.h"
#include <string.h>
#include <Windows.h>
#include "EnDecoding.h"
#include "logging.h"
#include <algorithm>

EncodedDataAccessor::EncodedDataAccessor(int n, const char** keys, const char** files)
{
	this ->fileCount = n;
	this ->keys.resize(n, NULL);
	this ->files.resize(n, NULL);
	this ->inPipes.resize(n, NULL);
	this ->outPipes.resize(n, NULL);
	this ->availableBytes.resize(n, 0);
	for(int i = 0; i < n; i++)
	{
		this ->keys[i] = (char*)malloc(strlen(keys[i]) + 1);
		strcpy(this ->keys[i], keys[i]);
		this ->files[i] = (char*)malloc(strlen(files[i]) + 1);
		strcpy(this ->files[i], files[i]);
		FILE* informationFile;
		fopen_s(&informationFile, files[i],"rb");
		fseek(informationFile, 0, SEEK_END);
		this ->availableBytes[i] = ftell(informationFile);
		fclose(informationFile);
	}
	tempBuffer = malloc(MAX_FILE_COUNT * MAX_BLOCK_SIZE);
}

void EncodedDataAccessor ::Start()
{
	for(int i = 0; i < fileCount; i++)
	{
		CreatePipe(&outPipes[i], &inPipes[i], NULL, MAX_THREAD_SIZE);
		void* argsForEncoding[3];
		argsForEncoding[0] = keys[i];
		argsForEncoding[1] = files[i];
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
		FILE* file = fopen(files[i], "rb");
		char buffer[102];
		fread(buffer, 1, 100, file);
		DWORD temp;
		WriteFile(inPipes[i], buffer, 100, &temp, NULL);		
		fclose(file);
	}

}

EncodedDataAccessor::~EncodedDataAccessor(void)
{
	for(int i = 0; i < fileCount; i++)
	{
		free(keys[i]);
		free(files[i]);
	}
	free(tempBuffer);
	
}
