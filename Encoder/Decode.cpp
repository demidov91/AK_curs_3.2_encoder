#include "Decode.h"
#include <Windows.h>
#include <process.h>
#include "Decoding.h"
#include "FSRestorer.h"

Decode::Decode(void)
{
}

int Decode ::start(char* inputFile, char* key, char* password, int thread)
{
	HANDLE rPipe = NULL;
	HANDLE wPipe = NULL;
	fopen_s(&input, inputFile, "rb");
	CreatePipe(&rPipe, &wPipe, NULL, 1000000);
	FSRestorer* decode_arg = new FSRestorer();
	decode_arg ->pipe = rPipe;
	DWORD threadId;
	_beginthread(decode_async, NULL, decode_arg);
	runDecoding(key, password, thread);
	CloseHandle(wPipe);
	while(decode_arg ->isAlive())
	{
		Sleep(1000);
	}
	return 0;
};

void Decode ::runDecoding(char*, char*, int)
{}