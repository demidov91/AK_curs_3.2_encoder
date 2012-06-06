#include "EnDecoding.h"
#include <fstream>
#include <iostream>
#include <Windows.h>
#include "CommunicationalStructures.h"
#include "FSDataEncoder.h"
#include <boost\filesystem.hpp>
#include <algorithm>
using namespace std;
using namespace boost ::filesystem;

void encode_async(void* in_args)
{
	ArgsForAsyncEncoder* args = (ArgsForAsyncEncoder*)in_args; 
	FSDataEncoder* threadParser = new FSDataEncoder(args ->key,  args ->pipe);
	char c_str_root[256];
	strcpy(c_str_root, args ->file);
	path* root = new path(string(c_str_root));
	threadParser ->encodeElement(root);
	delete root;
	delete threadParser;
	(*(args ->byteToTalk)) = 0;
}







/*void encode(FILE* file, PHANDLE output)
{
	char buffer[16];
	char wasRead=1;
	wasRead = fread(buffer, 1, 16, file);
	do
	{		
		DWORD varForProcessedBytes;
		WriteFile(*output, buffer, wasRead, &varForProcessedBytes, NULL);
		wasRead = fread(buffer, 1, 16, file);			
	}while(wasRead);
		
}*/





