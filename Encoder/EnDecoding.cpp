#include "EnDecoding.h"
#include <fstream>
#include <iostream>
#include <Windows.h>
#include "CommunicationalStructures.h"
#include <boost\filesystem.hpp>
#include <algorithm>
using namespace std;
using namespace boost ::filesystem;

CRITICAL_SECTION crSection;

void initCrSection()
{
	InitializeCriticalSectionAndSpinCount(&crSection, 0x00000400);	
}

void destroyCrSection()
{
	DeleteCriticalSection(&crSection);
}


void encode_element(const path* root, PHANDLE output)
{
	DWORD varForProcessedBytes;	
	char fsObjectName[256];
	strcpy(fsObjectName, root ->filename().string().c_str());
	char nameLength = strlen(fsObjectName);
	if(!is_directory(*root))
	{
		unsigned char filetype = FILE_BYTE;
		unsigned long int size = file_size(*root);
		
		WriteFile(*output, &filetype, 1, &varForProcessedBytes, NULL);
		WriteFile(*output, &size, sizeof(unsigned long int), &varForProcessedBytes, NULL);
		WriteFile(*output, &nameLength, sizeof(nameLength), &varForProcessedBytes, NULL);
		WriteFile(*output, fsObjectName, nameLength, &varForProcessedBytes, NULL);
		FILE* toEncode;
		char fileToEncode[256];
		strcpy(fileToEncode, root ->string().c_str());
		fopen_s(&toEncode, fileToEncode, "rb");
		encode(toEncode, output);
		fclose(toEncode);
	}
	else
	{
		recursive_directory_iterator begin(*root), end;
		int innerCount = count_if(begin, end,
    [](const directory_entry & d) {
		return !is_directory(d.path());});
		DWORD varForProcessedBytes;
		WriteFile(*output, (LPCVOID)DIRECTORY_BYTE, 1, &varForProcessedBytes, NULL);
		WriteFile(*output, (LPCVOID)innerCount, sizeof(innerCount), &varForProcessedBytes, NULL);
		WriteFile(*output, &nameLength, sizeof(nameLength), &varForProcessedBytes, NULL);
		WriteFile(*output, fsObjectName, nameLength, &varForProcessedBytes, NULL);
		for(recursive_directory_iterator it(*root); it != recursive_directory_iterator(); ++it)
		{
			if(!is_directory(*it))
			{
				encode_element(&(it ->path()), output);
			}
		}
					
	}

};




void encode_async(void* in_args)
{
	ArgsForAsyncEncoder* args = (ArgsForAsyncEncoder*)in_args; 
	FILE* file = fopen(args ->key, "rb");
	__int8 keyPart[16];
	fread(keyPart, 1, 16, file);
	__asm
	{
		movupd xmm0, keyPart
	}
	fread(keyPart, 1, 16, file);
	__asm
	{
		movupd xmm1, keyPart
	}
	fread(keyPart, 1, 16, file);
	__asm
	{
		movupd xmm2, keyPart
	}
	fread(keyPart, 1, 16, file);
	__asm
	{
		movupd xmm3, keyPart
	}
	fread(keyPart, 1, 16, file);
	__asm
	{
		movupd xmm4, keyPart
	}
	fread(keyPart, 1, 16, file);
	__asm
	{
		movupd xmm5, keyPart
	}
	fread(keyPart, 1, 16, file);
	__asm
	{
		movupd xmm6, keyPart
	}
	fclose(file);
	char c_str_root[256];
	strcpy(c_str_root, args ->file);
	EnterCriticalSection(&crSection); 
	path* root = new path(string(c_str_root));
	LeaveCriticalSection(&crSection); 
	encode_element(root, args ->pipe);
	delete root;
	(*(args ->byteToTalk)) = 0;
}




void encode(FILE* file, PHANDLE output)
{
	__int16 step = 0;
	char buffer[16];
	char toWatch[16];
	__int16 wasRead=1;
	__asm{
		startReading:
	}
			wasRead = fread(buffer, 1, 16, file);
		__asm{			
			mov BX, wasRead
			cmp BX, 0
			JZ stopReading
			movups xmm7, buffer
			mov CX, step
isIt0:
			cmp CX, 0
			JNZ isIt1
			xorps xmm7, xmm0 
			jmp writeOut
isIt1:
			cmp CX, 1
			JNZ isIt2
			xorps xmm7, xmm1 
			jmp writeOut
isIt2:
			cmp CX, 2
			JNZ isIt3
			xorps xmm7, xmm2
			jmp writeOut
isIt3:
			cmp CX, 3
			JNZ isIt4
			xorps xmm7, xmm3
			jmp writeOut
isIt4:
			cmp CX, 4
			JNZ isIt5
			xorps xmm7, xmm4 
			jmp writeOut
isIt5:
			cmp CX, 5
			JNZ isIt6
			xorps xmm7, xmm5 
			jmp writeOut
isIt6:
			xorps xmm7, xmm6 
			jmp writeOut
writeOut:
			movups buffer, xmm7

			inc CX
			cmp CX, 7
			JNZ toStream
			mov CX, 0
			
toStream:
			mov step, CX
		}
			DWORD varForProcessedBytes;
			WriteFile(*output, buffer, wasRead, &varForProcessedBytes, NULL);
			
			
		__asm{
			jmp startReading
stopReading:
		}
		
}