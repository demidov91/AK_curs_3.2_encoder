#include "EnDecoding.h"
#include <sstream>
#include <process.h>
#include <stdio.h>
#include <fstream>
#include "tests.h"
#include <Windows.h>
#include <stdlib.h>
#include "LengthProducer.h"
#include "CommunicationalStructures.h"
#include <iostream>
#include <conio.h>
#include "EncodedDataAccessor.h"
#include "logging.h"
#include "Encoder.h"
#include "MapEncoder.h"
#include "FilesystemHelper.h"
#include "RandomGenerator.h"
#include "DataEncoder.h"
#include "FSDataEncoder.h"
#include "ByteEncoder.h"
using namespace std;



void Friendly ::test_DataEncoder_encodeBlock()
{
	FILE* keyFile = fopen("test\\DataEncoder\\key.key", "rb");
	DataEncoder tester(keyFile);
	fclose(keyFile);
	char initBuffer[128];
	for(int i = 0; i < 128; i++)
	{
		initBuffer[i] = rand();
	}
	initBuffer[127] = 0;
	char bufferToEncode[128];
	memcpy(bufferToEncode, initBuffer, 128);
	for(int i = 0; i < 128; i += 16)
	{
		tester.encodeBlock(bufferToEncode+i);
	}
	keyFile = fopen("test\\DataEncoder\\key.key", "rb");
	tester = DataEncoder(keyFile);
	fclose(keyFile);
	for(int i = 0; i < 128; i += 16)
	{
		tester.encodeBlock(bufferToEncode+i);
	}
	if(strcmp(initBuffer, bufferToEncode))
	{
		log_test("Encode Block");
	}
}

void Friendly ::test_FSDataEncoder_pushToEncode()
{
	HANDLE rPipe = 0;
	HANDLE wPipe = 0;
	CreatePipe(&rPipe, &wPipe, 0, 100);
	
	FSDataEncoder tester("test\\DataEncoder\\blank.key", &wPipe);
	char* initialSequence = "12345678901234567890123456789012345678901234567890";
	tester.pushToEncode(initialSequence, 5);
	DWORD read = 0;
	DWORD available = 0;
	char buffer[100];
	PeekNamedPipe(rPipe, buffer, 100, &read, &available, 0);
	if(available !=0)
	{
		log_test("test_FSDataEncoder_pushToEncode");
		return;
	}
	tester.pushToEncode(initialSequence+5, 11);
	PeekNamedPipe(rPipe, buffer, 100, &read, &available, 0);
	if(available !=16)
	{
		log_test("test_FSDataEncoder_pushToEncode");
		return;
	}
	tester.pushToEncode(initialSequence+16, 15);
	PeekNamedPipe(rPipe, buffer, 100, &read, &available, 0);
	if(available !=16)
	{
		log_test("test_FSDataEncoder_pushToEncode");
		return;
	}
	tester.pushToEncode(initialSequence+31, 19);
	PeekNamedPipe(rPipe, buffer, 100, &read, &available, 0);
	if(available !=48)
	{
		log_test("test_FSDataEncoder_pushToEncode");
		return;
	}
	CloseHandle(wPipe);
	CloseHandle(rPipe);
}

void prepair_file_for_FSDataEncoder_encodeFile(const char* filename, const char* text)
{
	FILE* file = fopen(filename, "wb");
	fwrite(text, 1, strlen(text) + 1, file);
	fclose(file);
}


void Friendly ::test_FSDataEncoder_encodeFile()
{
	HANDLE rPipe = 0;
	HANDLE wPipe = 0;
	CreatePipe(&rPipe, &wPipe, 0, 0);
	FSDataEncoder* tester = new FSDataEncoder("test\\FSDataEncoder\\blank.key", &wPipe);
	char* initialSequence = "12345678901234567890123456789012345678901234567890";
	tester ->pushToEncode(initialSequence, 50);
	
	const char* filename = "test\\FSDataEncoder\\input1.txt";
	const char* text = "Mother was washing windows";
	prepair_file_for_FSDataEncoder_encodeFile(filename, text);

	FILE* file;
	fopen_s(&file, filename, "rb");
	tester ->encodeFile(file);
	fclose(file);
	delete tester;
	char buffer[100];
	DWORD read = 0;
	ReadFile(rPipe, buffer, 50, &read, 0);
	buffer[50] = 0;
	if(strcmp(buffer, initialSequence))
	{
		log_test("test_FSDataEncoder_encodeFile");
		return;
	}
	ReadFile(rPipe, buffer, strlen(text)+1, &read, 0);
	if(strcmp(buffer, text))
	{
		log_test("test_FSDataEncoder_encodeFile");
		return;
	}
	CloseHandle(wPipe);
	CloseHandle(rPipe);
}

void test_ReadToBuffer(HANDLE pipe, char*buffer, int size)
{
	DWORD wasAvailable = 0;
	while(size)
	{
		DWORD toRead = 0;
		DWORD bytesProcessed = 0;			
		
		if(wasAvailable < size)
		{
			PeekNamedPipe(pipe, buffer, size, &toRead, &wasAvailable, NULL);			
		}
		else
		{
			toRead = size;			
		}		
		if (!toRead)
		{
			Sleep(1000);
		}
		else{
		ReadFile(pipe,buffer, toRead, &bytesProcessed, NULL);
		wasAvailable -= toRead;
		size -= toRead;
		buffer += toRead;	
		}
	}

}


void test_EnDecoder_async_one_way(const char* inp1, const char* inp2, const char* outp1, const char* outp2)
{
	
	FILE* file;
	fopen_s(&file, inp1, "rb");
	fseek(file, 0, SEEK_END);
	long size1 = ftell(file);
	fclose(file);

	HANDLE rPipe[2], wPipe[2];

	CreatePipe(&rPipe[0], &wPipe[0], NULL, 100000000);
	ArgsForAsyncEncoder* args1 = (ArgsForAsyncEncoder*)malloc(sizeof(ArgsForAsyncEncoder));
	args1 ->key = "key1.txt";
	args1 ->file = inp1;
	args1 ->pipe = &wPipe[0];
	char byteToTalk1 = 1;
	args1 ->byteToTalk = &byteToTalk1;
	_beginthread(encode_async, 10000, (void*)args1);

	fopen_s(&file,inp2, "rb");
	fseek(file, 0, SEEK_END);
	long size2 = ftell(file);
	fclose(file);


	CreatePipe(&rPipe[1], &wPipe[1], NULL, 100000000);
	ArgsForAsyncEncoder* args2 = (ArgsForAsyncEncoder*)malloc(sizeof(ArgsForAsyncEncoder));
	args2 ->key = "key2.txt";
	args2 ->file = inp2;
	args2 ->pipe = &wPipe[1];
	char byteToTalk2 = 1;
	args2 ->byteToTalk = &byteToTalk2;
	_beginthread(encode_async, 10000, args2);


	ofstream outFile1(outp1, ostream ::binary);
	ofstream outFile2(outp2, ostream ::binary);

	unsigned char oneByteBuffer = 0;
	unsigned long int fileLengthBuffer = 0;
	test_ReadToBuffer(rPipe[0],(char*)&oneByteBuffer, 1);
	test_ReadToBuffer(rPipe[0],(char*)&fileLengthBuffer, 4);
	test_ReadToBuffer(rPipe[0],(char*)&oneByteBuffer, 1);
	char forFilename[257];
	test_ReadToBuffer(rPipe[0], forFilename, strlen(inp1));
	forFilename[oneByteBuffer] = 0;
	const int BUFFER_SIZE = 200000;
	char buffer[BUFFER_SIZE];
	test_ReadToBuffer(rPipe[0], buffer, size1);
	outFile1.write(buffer, size1);
	outFile1.close();


	test_ReadToBuffer(rPipe[1],(char*)&oneByteBuffer, 1);
	test_ReadToBuffer(rPipe[1],(char*)&fileLengthBuffer, 4);
	test_ReadToBuffer(rPipe[1],(char*)&oneByteBuffer, 1);
	test_ReadToBuffer(rPipe[1], forFilename, strlen(inp2));
	forFilename[oneByteBuffer] = 0;
	test_ReadToBuffer(rPipe[1], buffer, size2);
	outFile2.write(buffer, size2);
	outFile2.close();


	free(args1);
	free(args2);
	CloseHandle(wPipe[0]);
	CloseHandle(wPipe[1]);
	CloseHandle(rPipe[0]);
	CloseHandle(rPipe[1]);

}


/**
encodes input1.txt, input2.txt into output1.txt and outout2.txt and decodes them back into input1_2.txt and input2_2.txt 
This test will not work correctly but it won't raise exception too.
*/
void test_EnDecoder_async()
{
	FILE* key1, *key2;
	fopen_s(&key1, "key1.txt", "wb");
	fopen_s(&key2, "key2.txt", "wb");
	for(int i = 0; i < 56; i++)
	{
		int buff = rand();
		fwrite(&buff, sizeof(int), 1, key1);
		buff = rand();
		fwrite(&buff, sizeof(int), 1, key2);
	}
	fclose(key1);
	fclose(key2);

	test_EnDecoder_async_one_way("input1.txt", "input2.txt", "output1.txt", "outptu2.txt");
	test_EnDecoder_async_one_way("output1.txt", "outptu2.txt", "input1_2.txt", "input2_2.txt");
}


void Friendly ::test_FSDataEncoder_encodeElement()
{
	const char* inp1 = "test\\FSDataEncoder\\encElement1.txt";
	const char* inp2 = "test\\FSDataEncoder\\encElement2.txt";
	FILE* file;
	fopen_s(&file, inp1, "rb");
	fseek(file, 0, SEEK_END);
	long size1 = ftell(file);
	fclose(file);

	HANDLE rPipe[2], wPipe[2];

	CreatePipe(&rPipe[0], &wPipe[0], NULL, 100000000);
	ArgsForAsyncEncoder* args1 = (ArgsForAsyncEncoder*)malloc(sizeof(ArgsForAsyncEncoder));
	args1 ->key = "test\\FSDataEncoder\\blank.key";
	args1 ->file = inp1;
	args1 ->pipe = &wPipe[0];
	char byteToTalk1 = 1;
	args1 ->byteToTalk = &byteToTalk1;
	_beginthread(encode_async, 10000, (void*)args1);

	fopen_s(&file,inp2, "rb");
	fseek(file, 0, SEEK_END);
	long size2 = ftell(file);
	fclose(file);


	CreatePipe(&rPipe[1], &wPipe[1], NULL, 100000000);
	ArgsForAsyncEncoder* args2 = (ArgsForAsyncEncoder*)malloc(sizeof(ArgsForAsyncEncoder));
	args2 ->key = "test\\FSDataEncoder\\blank.key";
	args2 ->file = inp2;
	args2 ->pipe = &wPipe[1];
	char byteToTalk2 = 1;
	args2 ->byteToTalk = &byteToTalk2;
	_beginthread(encode_async, 10000, args2);

	unsigned char oneByteBuffer = 0;
	unsigned long int fileLengthBuffer = 0;
	test_ReadToBuffer(rPipe[0],(char*)&oneByteBuffer, 1);
	if (oneByteBuffer != FILE_BYTE)
	{
		log_test("async encoding");
		return;
	}
	test_ReadToBuffer(rPipe[0],(char*)&fileLengthBuffer, 4);
	if (fileLengthBuffer != size1)
	{
		log_test("async encoding");
		return;
	}
	test_ReadToBuffer(rPipe[0],(char*)&oneByteBuffer, 1);
	if (oneByteBuffer < 1)
	{
		log_test("async encoding");
		return;
	}
	char forFilename[257];
	test_ReadToBuffer(rPipe[0], forFilename, oneByteBuffer);
	forFilename[oneByteBuffer] = 0;
	if(strcmp(path(inp1).filename().string().c_str() ,forFilename))
	{
		log_test("async encoding");
		return;
	}
	const int BUFFER_SIZE = 200000;
	char buffer[BUFFER_SIZE];
	test_ReadToBuffer(rPipe[0], buffer, size1);
	

	test_ReadToBuffer(rPipe[1],(char*)&oneByteBuffer, 1);
	if (oneByteBuffer != FILE_BYTE)
	{
		log_test("async encoding");
		return;
	}
	test_ReadToBuffer(rPipe[1],(char*)&fileLengthBuffer, 4);
	if (fileLengthBuffer != size2)
	{
		log_test("async encoding");
		return;
	}
	test_ReadToBuffer(rPipe[1],(char*)&oneByteBuffer, 1);
	if (oneByteBuffer < 1)
	{
		log_test("async encoding");
			return;
	}
	test_ReadToBuffer(rPipe[1], forFilename, oneByteBuffer);
	forFilename[oneByteBuffer] = 0;
	if(strcmp(path(inp2).filename().string().c_str() ,forFilename))
	{
		log_test("async encoding");
		return;
	}
	test_ReadToBuffer(rPipe[1], buffer, size2);
	

	free(args1);
	free(args2);
	CloseHandle(wPipe[0]);
	CloseHandle(wPipe[1]);
	CloseHandle(rPipe[0]);
	CloseHandle(rPipe[1]);
	
}




void test_EncodedDataAccessor_prepairFiles(vector<const string> names)
{
	for(char i = 0; i < 3; i++)
	{
		FILE* file;
		fopen_s(&file, names[i].c_str(), "wb");
		for(int j = 0; j < 100; j++)
		{
			fwrite(&i, 1, 1, file);
		}
		fclose(file);
	}
}




void test_EncodedDataAccessor_genPointers(BYTE* pointers, BYTE* lengthes)
{
	for(char i = 0; i < 3; i++)
	{
		pointers[2*i] = i;
		pointers[2*i+1] = lengthes[i];
	}
}


void test_EncodedDataAccessor_checkData(BYTE* buffer)
{
	bool ok = true;
	for(int i = 0; i < 9; i++)
	{
		ok &= buffer[i] == 0;
	}
	for(int i = 9; i < 90; i++)
	{
		ok &= buffer[i] == 1;
	}
	for(int i = 90; i < 180; i++)
	{
		ok &= buffer[i] == 2;
	}


	for(int i = 180; i < 270; i++)
	{
		ok &= buffer[i] == 0;
	}
	for(int i = 270; i < 282; i++)
	{
		ok &= buffer[i] == 1;
	}
	

	for(int i = 282; i < 283; i++)
	{
		ok &= buffer[i] == 0;
	}
	for(int i = 285; i < 292; i++)
	{
		ok &= buffer[i] == 1;
	}
	for(int i = 294; i < 304; i++)
	{
		ok &= buffer[i] == 2;
	}
	cout << ok ? log_test("EncodedDataAccessor: ok") : log_test("EncodedDataAccessor: fail");
}

void test_EncodedDataAccessor()
{
	vector<const string> names;
	names.push_back(string("test\\one"));
	names.push_back(string("test\\two"));
	names.push_back(string("test\\three"));
	vector<const path> keys;
	keys.push_back(path());
	keys.push_back(path());
	keys.push_back(path());
	

	test_EncodedDataAccessor_prepairFiles(names);
	EncodedDataAccessor* dataAccessor = new EncodedDataAccessor();
	dataAccessor ->create(&keys, &names);
	dataAccessor ->__testStart();
	BYTE* buffer = new BYTE[306];
	BYTE pointers[6];
	BYTE lengthes[] = {3, 27, 30};
	test_EncodedDataAccessor_genPointers(pointers, lengthes);
	dataAccessor ->getData(buffer, pointers, 3);
	buffer += 180;
	lengthes[0] = 30;
	lengthes[1] = 4;
	lengthes[2] = 0;
	test_EncodedDataAccessor_genPointers(pointers, lengthes);
	dataAccessor ->getData(buffer, pointers, 3);
	buffer += 102;
	lengthes[0] = 1;
	lengthes[1] = 3;
	lengthes[2] = 4;
	test_EncodedDataAccessor_genPointers(pointers, lengthes);
	dataAccessor ->getData(buffer, pointers, 3);
	buffer -= 282;
	test_EncodedDataAccessor_checkData(buffer);
	delete[] buffer;
	delete dataAccessor;
}


void test_EncoderCountpercentage()
{
	Encoder testing;
	int input[] = {101, 200, 300, 500, 400};
	int output[5] = {0,0,0,0,0};
	for(int i = 0; i < 5; i++)
	{
		output[i] = testing.__CountDataBlock(input[i]);
	}
	if(output[0] != 250 || output[1] != 3 || output[2] != 1 || output[3] != 0 || output[4] != 1)
	{
		cerr << "Error in EncoderCountpercentage"<< output << endl;
	}
}

void test_LengthGenerator_GetFSObjectSize()
{
	vector<const string> names;
	LengthProducer tester;
	if (GetFSObjectSize(&string("test/value_test1")) != 68)
	{
		log_error("test/value_test1 error");		
	}
	if (GetFSObjectSize(&string("test/value_test2")) != 119)
	{
		log_error("test/value_test2 error");		
	}
	if (GetFSObjectSize(&string("test/value_test3")) != 22)
	{
		log_error("test/value_test3 error");		
	}
}

void Friendly ::test_formCollectionOfAvailableKeys()
{
	Encoder tester;
	tester.formCollectionOfAvailableKeys("test\\keys_only_names");
	if(tester.availableKeys.size() != 2)
	{
		log_error("test_formCollectionOfAvailableKeys");
	}
	cout << "Ok? Keys:" << endl <<tester.availableKeys.front().filename().string() << endl;
	tester.availableKeys.pop_front();
	cout <<tester.availableKeys.front().filename().string() << endl;
}

void Friendly ::test_Encoder_keyForThread()
{
	Encoder tester;
	tester.formCollectionOfAvailableKeys("test/keys_only_names");
	if(tester.keyForThread(&string("key.")) != path())
	{
		log_test("keyForThread incorrect");
	}
	if(tester.keyForThread(&string("key")) != path())
	{
		log_test("keyForThread incorrect");
	}
	if(tester.keyForThread(&string("key.key")) == path("key.key.key"))
	{
		log_test("keyForThread incorrect");
	}
	if(tester.keyForThread(&string("key.key")) != path())
	{
		log_test("keyForThread incorrect");
	}
	if(tester.keyForThread(&string("1")) == path("1.key"))
	{
		log_test("keyForThread incorrect");
	}

}


void Friendly ::test_Encoder_generateKeyForThread()
{
	string threadname("test\\generate_keys\\thraedname");
	int number = 12;
	Encoder tester;
	tester.generateKeyForThread(&threadname, number);
	path target("test\\generate_keys\\thraedname_12.key"); 
	if(!exists(target))
	{
		log_test("test_Encoder_generateKeyForThread");
		return;
	}
	if(file_size(target) != 112)
	{
		log_test("test_Encoder_generateKeyForThread");
		return;
	}
}

void Friendly ::test_Encoder_renameKey()
{
	string threadname("test\\generate_keys\\for_renaming");
	int number = 5;
	Encoder tester;
	tester.generateKeyForThread(&threadname, number);
	path target("test\\generate_keys\\for_renaming_5.key"); 
	
	path result = tester.renameKey(&target, &path(threadname).filename().string(), 6);
	target = path("test\\generate_keys\\for_renaming_6.key");
	if(target != result)
	{
		log_test("test_Encoder_renameKey");
		return;
	}
	if(!exists(target))
	{
		log_test("test_Encoder_renameKey");
		return;
	}
}

void Friendly ::test_MapEncoder_costructor()
{
	vector<const string> keys;
	keys.push_back("1");
	keys.push_back("");
	keys.push_back("2");
	MapEncoder tester;
	tester.create(&keys);
	for(int i = 0; i < keys.size(); i++)
	{
		if(keys[i].compare(tester.threads[i] ->key))
		{
			log_test("MapEncoder_costructor");
			return;
		}
	}
}

void test_LengthProducer_prepareFiles(vector<const string>* names, vector<int>* lengthes)
{
	char buffer[200000];
	memset(buffer, 0, 200000);
	for(int i = 0; i < 3; i++)
	{
		FILE* file = fopen((*names)[i].c_str(), "wb");
		fwrite(buffer, 1, (*lengthes)[i], file);
		fclose(file);
	}

}

void Friendly::test_LengthProducer_constructor()
{
	const char* names[] =  {"test\\length\\construct1.txt", "test\\length\\construct2.txt", "test\\length\\construct3.txt"};
	vector<const string> v_names;
	for (int i = 0; i< 3; i++)
	{
		v_names.push_back(string(names[i]));
	}
	vector<int> lengthes;
	lengthes.push_back(129980);
	lengthes.push_back(12968);
	lengthes.push_back(25979);
	test_LengthProducer_prepareFiles(&v_names, &lengthes);
	LengthProducer producer;
	producer.create(&v_names, 13);
	if(producer.bytesAvailable[0] != 10000 || producer.bytesAvailable[1] != 1000 || producer.bytesAvailable[2] != 2000)
	{
		log_test("lengthProducer constructor");		
	}	
}

bool test_LengthProducer_getNext_checkAnswer(LengthProducer* tester)
{
	Lengthes nextLengthes = tester ->getNextLengthes();
	if(nextLengthes.count != 38 || nextLengthes.answer.size() != 3)
		return false;
	vector<ThreadLengthPair>* answer = &nextLengthes.answer;
	if((*answer)[0].length != 230 || (*answer)[1].length != 0 || (*answer)[2].length != 26)
		return false;

	nextLengthes = tester ->getNextLengthes();
	if(nextLengthes.count != 1 || nextLengthes.answer.size() != 3)
		return false;
	answer = &nextLengthes.answer;
	if((*answer)[0].length != 244 || (*answer)[1].length != 0 || (*answer)[2].length != 12)
		return false;

	nextLengthes = tester ->getNextLengthes();
	if(nextLengthes.count != 11 || nextLengthes.answer.size() != 3)
		return false;
	answer = &nextLengthes.answer;
	if((*answer)[0].length != 86 || (*answer)[1].length != 85 || (*answer)[2].length != 85)
		return false;

	nextLengthes = tester ->getNextLengthes();
	if(nextLengthes.count != 1 || nextLengthes.answer.size() != 3)
		return false;
	answer = &nextLengthes.answer;
	if((*answer)[0].length != 70 || (*answer)[1].length != 65 || (*answer)[2].length != 65)
		return false;

	if(tester ->getNextLengthes().count != 0)
		return false;

	return true;
}


void Friendly::test_LengthProducer_getNext()
{
	const char* names[] =  {"test\\length\\construct1.txt", "test\\length\\construct2.txt", "test\\length\\construct3.txt"};
	vector<const string> v_names;
	for (int i = 0; i< 3; i++)
	{
		v_names.push_back(string(names[i]));
	}
	vector<int> lengthes;
	lengthes.push_back(129980);
	lengthes.push_back(12968);
	lengthes.push_back(25979);
	test_LengthProducer_prepareFiles(&v_names, &lengthes);
	LengthProducer producer;
	producer.create(&v_names, 13);
	if(!test_LengthProducer_getNext_checkAnswer(&producer))
	{
		log_test("LengthProducer_getNext");
	}	
}


void Friendly ::test_RandomGenerator_shuffle()
{
	RandomGenerator tester;
	tester.create(2);
	BYTE data[BLOCK_COUNT*2];
	for(int i = 0; i < BLOCK_COUNT; i++)
	{
		data[2*i]=i;
		data[2*i+1]=i;
	}
	tester.shuffle(data);
	char hits[BLOCK_COUNT];
	memset(hits, 0, 256);
	for(int i = 0; i < BLOCK_COUNT; i++)
	{
		if(data[tester.map[i]*2] != i || data[tester.map[i]*2 + 1] != i || hits[tester.map[i]])
		{
			log_test("RandomGenerator shuffle");
			return;
		}
		hits[tester.map[i]] = 1;
	}
}

void Friendly ::test_ThreadMapEncoder_encodeByte()
{
	char* original = "Mother was washing window";
	int n = strlen(original) + 1;
	char* forTest = new char[n];
	memcpy(forTest, original, n);
	
	for (int twoWay = 0; twoWay < 2; twoWay++)
	{
		ByteEncoder* tester = new ByteEncoder(&string("12345"));
		for (int i = 0; i < n; i++)
		{
			tester ->encodeByte(&forTest[i]);			
		}
		delete tester;
	}
	if(strcmp(original, forTest))
	{
		log_test("ThreadMapEncoder_encodeByte");
		return;
	}

	for (int twoWay = 0; twoWay < 2; twoWay++)
	{
		ByteEncoder* tester = new ByteEncoder(&string());
		for (int i = 0; i < n; i++)
		{
			tester ->encodeByte(&forTest[i]);			
		}
		delete tester;
	}
	if(strcmp(original, forTest))
	{
		log_test("ThreadMapEncoder_encodeByte");
		return;
	}

	
	delete[] forTest;
}

void makeBlankKey()
{
	FILE* file = fopen("test\\blankKey.key", "wb");
	for(int i = 0; i < 112; i++)
	{
		char zero = 0;
		fwrite(&zero, 1, 1, file);
	}
	fclose(file);
}


void beginTests()
{
	Friendly ::test_DataEncoder_encodeBlock();
	Friendly ::test_FSDataEncoder_pushToEncode();
	Friendly ::test_FSDataEncoder_encodeElement();
	Friendly ::test_FSDataEncoder_encodeFile();
	test_EnDecoder_async();
	test_EncodedDataAccessor();
	test_EncoderCountpercentage();
	test_LengthGenerator_GetFSObjectSize();
	Friendly ::test_formCollectionOfAvailableKeys();
	Friendly ::test_Encoder_keyForThread();
	Friendly ::test_Encoder_generateKeyForThread();
	Friendly ::test_Encoder_renameKey();
	Friendly ::test_MapEncoder_costructor();
	Friendly ::test_LengthProducer_constructor();
	Friendly ::test_LengthProducer_getNext();
	Friendly ::test_RandomGenerator_shuffle();
	Friendly ::test_ThreadMapEncoder_encodeByte();
	getch();
}