#pragma once
#include <stdio.h>
#include <Windows.h>
#include "DataEncoder.h"
#include <boost\filesystem.hpp>
using namespace std;
using namespace boost ::filesystem;
#define DIRECTORY_BYTE 1
#define FILE_BYTE 2

#include "tests.h"


class FSDataEncoder{
private:
	DataEncoder* processor;
	char buffer[16];
	char bytesInBuffer;
	PHANDLE pipe;
	FILE* keyFile;

	void flushBuffer();
	void pushToEncode(void* buffer, int length);
	void encodeFile(FILE* file);
public:
	FSDataEncoder(const char* keyFilename, PHANDLE pipe);
	void encodeElement(const path* root);
	~FSDataEncoder();
	friend Friendly;
};