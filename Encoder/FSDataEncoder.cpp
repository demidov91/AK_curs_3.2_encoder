#include "FSDataEncoder.h"
#include "logging.h"


FSDataEncoder ::FSDataEncoder(const char* keyFilename, PHANDLE pipe)
{
	this ->pipe = pipe;
	fopen_s(&keyFile, keyFilename, "rb");
	this ->processor = new DataEncoder(keyFile);
	fclose(keyFile);
	bytesInBuffer = 0;
}


void FSDataEncoder ::encodeElement(const path* root)
{
	DWORD varForProcessedBytes;	
	char fsObjectName[256];
	strcpy(fsObjectName, root ->filename().string().c_str());
	unsigned char fileType;
	unsigned long int size;
	char nameLength = strlen(fsObjectName);
	if(!is_directory(*root))
	{
		fileType = FILE_BYTE;
		size = file_size(*root);
		pushToEncode(&fileType, 1);
		pushToEncode(&size, sizeof(unsigned long int));
		pushToEncode(&nameLength, sizeof(nameLength));
		pushToEncode(&fsObjectName, nameLength);
		FILE* toEncode;
		char fileToEncode[256];
		strcpy(fileToEncode, root ->string().c_str());
		fopen_s(&toEncode, fileToEncode, "rb");
		encodeFile(toEncode);
		fclose(toEncode);
	}
	else
	{
		recursive_directory_iterator begin(*root), end;
		int innerCount = count_if(begin, end,
    [](const directory_entry & d) {
		return !is_directory(d.path());});
		fileType = DIRECTORY_BYTE;
		size = innerCount;
		pushToEncode(&fileType, 1);
		pushToEncode(&size, sizeof(unsigned long int));
		pushToEncode(&nameLength, sizeof(nameLength));
		pushToEncode(&fsObjectName, nameLength);
		for(recursive_directory_iterator it(*root); it != recursive_directory_iterator(); ++it)
		{
			if(!is_directory(*it))
			{
				encodeElement(&(it ->path()));
			}
		}
					
	}

};

void FSDataEncoder ::flushBuffer()
{
	DWORD bytesWritten = 0;
	processor ->encodeBlock(buffer);
	WriteFile(*pipe, buffer, 16, &bytesWritten, 0);
	if(bytesWritten < 16)
	{
		log_error("flush buffer. written less than 16");
	}
}

void FSDataEncoder::pushToEncode(void* buff, int length)
{
	char* buffer = (char*)buff;
	char* endBuffer = buffer + length;
	if(length + bytesInBuffer >= 16)
	{
		memcpy(this ->buffer + bytesInBuffer, buffer, 16 - bytesInBuffer);
		flushBuffer();
		buffer += 16 - bytesInBuffer;
		bytesInBuffer = 0;		
	}
	while(endBuffer - buffer >= 16)
	{
		memcpy(this ->buffer, buffer, 16);
		flushBuffer();
		buffer += 16;
	}
	memcpy(this ->buffer + bytesInBuffer, buffer, endBuffer - buffer);
	bytesInBuffer += (endBuffer - buffer);
}


void FSDataEncoder ::encodeFile(FILE* file)
{
	char bytesRead = fread(this ->buffer+bytesInBuffer, 1, 16-bytesInBuffer, file);
	flushBuffer();
	bytesInBuffer = 0;
	bytesRead = fread(this ->buffer, 1, 16, file);
	while(bytesRead)
	{
		flushBuffer();
		bytesRead = fread(this ->buffer, 1, 16, file);
	}

}


FSDataEncoder ::~FSDataEncoder()
{
	delete processor;
}