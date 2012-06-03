#pragma once
#include <stdio.h>


class Decode
{
private:
	FILE* input;
public:
	Decode(void);
	int start(char* input, char* key, char* verbalKey, int thread);	
	void runDecoding(char*, char*, int);
};

