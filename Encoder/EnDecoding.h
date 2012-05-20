#pragma once
#include <stdio.h>
#include <Windows.h>
using namespace std;


/**
in_args[0] - key file name
in_args[1] - inner file name
in_args[2] - outer pipe handle
*/
void encode_async(void* in_args);
void encode(FILE* file, PHANDLE output);