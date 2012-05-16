#pragma once
#include <stdio.h>
#include <Windows.h>
using namespace std;

void encode_async(void* in_args);
void encode(FILE* file, PHANDLE output);