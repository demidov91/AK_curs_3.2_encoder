#include "EnDecoding.h"
#include <fstream>
#include <iostream>
#include <Windows.h>
using namespace std;


void encode_async(void* in_args)
{
	void** args = (void**)in_args; 
	FILE* file = fopen((char*)args[0], "rb");
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

	file = fopen((const char*)args[1], "rb");
	encode(file, (PHANDLE)(args[2]));
	fclose(file);
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