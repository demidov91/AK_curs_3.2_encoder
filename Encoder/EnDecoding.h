#pragma once
#include <stdio.h>
#include <Windows.h>
#define DIRECTORY_BYTE 1
#define FILE_BYTE 2
using namespace std;


void encode_async(void* in_args);
void encode(FILE* file, PHANDLE output);

