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
using namespace std;




/*
encodes with zero key input1.txt into temp.txt and than decodes it into output.txt
*/
bool test_EnDecoder()
{
	FILE* file;
	HANDLE rPipe;
	HANDLE wPipe;
	
	fopen_s(&file, "input.txt", "rb");
	fseek(file, 0, SEEK_END);
	int size = ftell(file);
	fseek(file, 0, SEEK_SET);
	CreatePipe(&rPipe, &wPipe, NULL, 5000);
	encode(file, &wPipe);
	fclose(file);
	
	char* buffer = new char [size];
	DWORD processedNumber;
	ReadFile(rPipe, buffer, size, &processedNumber, NULL);
	CloseHandle(rPipe);
	CloseHandle(wPipe);

	ofstream tempFile("temp.txt", ostream ::binary);
	tempFile.write(buffer, size);
	tempFile.close();

	fopen_s(&file, "temp.txt", "rb");
	CreatePipe(&rPipe, &wPipe, NULL, 5000);
	
	encode(file, &wPipe);
	fclose(file);
	ReadFile(rPipe, buffer, size, &processedNumber, NULL);
	ofstream tempFile2("output.txt", ostream ::binary);
	tempFile2.write(buffer, size);
	tempFile2.close();

	delete[] buffer;
	return true;

}


void test_EnDecoder_async_one_way(const char* inp1, const char* inp2, const char* outp1, const char* outp2)
{
	
	FILE* file;
	fopen_s(&file, inp1, "rb");
	fseek(file, 0, SEEK_END);
	long size1 = ftell(file);
	fclose(file);

	HANDLE rPipe[2], wPipe[2];

	CreatePipe(&rPipe[0], &wPipe[0], NULL, 5000); 
	void* args[3];
	args[0] = malloc(sizeof(char) * strlen("key1.txt") + 1);
	strcpy((char*)(args[0]), "key1.txt");
	args[1] = malloc(sizeof(char) * strlen(inp1) + 1);
	strcpy((char*)(args[1]), inp1);
	args[2] = &wPipe[0];

	
	_beginthread(encode_async, 100, (void*)args); 


	

	fopen_s(&file,inp2, "rb");
	fseek(file, 0, SEEK_END);
	long size2 = ftell(file);
	fclose(file);


	CreatePipe(&rPipe[1], &wPipe[1], NULL, 5000); 
	void* args2[3];
	args2[0] = malloc(sizeof(char) * strlen("key2.txt") + 1);
	strcpy((char*)(args2[0]), "key2.txt");
	args2[1] = malloc(sizeof(char) * strlen(inp2) + 1);
	strcpy((char*)(args2[1]), inp2);
	args2[2] = &wPipe[1];
	_beginthread(encode_async, 100, args2); 

	
	ofstream outFile1(outp1, ostream ::binary);
	ofstream outFile2(outp2, ostream ::binary);
	const int BUFFER_SIZE = 100;
	char buffer[BUFFER_SIZE];
	DWORD wasAvailable[2];
	for(int i = 0; i < 2; i++)
	{
		wasAvailable[i] = 0;
	}
	while(size1 > 0 || size2 > 0)
	{
		DWORD toRead;
		DWORD bytesProcessed;			
		if(size1 > 0)
		{
			if(wasAvailable[0] < BUFFER_SIZE)
			{
				PeekNamedPipe(rPipe[0], buffer, BUFFER_SIZE, &toRead, &wasAvailable[0], NULL);			
			}
			else
			{
				toRead = BUFFER_SIZE;			
			}
		
			ReadFile(rPipe[0],buffer, toRead, &bytesProcessed, NULL);
			wasAvailable[0] -= toRead;
			outFile1.write(buffer, bytesProcessed);
			size1 -= toRead;
		}

		if(size2 > 0)
		{
			if(wasAvailable[1] < BUFFER_SIZE)
			{
				PeekNamedPipe(rPipe[1], buffer, BUFFER_SIZE, &toRead, &wasAvailable[1], NULL);			
			}
			else
			{
				toRead = BUFFER_SIZE;			
			}
		
			ReadFile(rPipe[1],buffer, toRead, &bytesProcessed, NULL);
			wasAvailable[1] -= toRead;
			outFile2.write(buffer, bytesProcessed);
			size2 -= toRead;
		}
	}
	outFile1.close();
	outFile2.close();
	free(args[0]);
	free(args2[0]);
	free(args[1]);
	free(args2[1]);
	CloseHandle(wPipe[0]);
	CloseHandle(wPipe[1]);
	CloseHandle(rPipe[0]);
	CloseHandle(rPipe[1]);
}


/**
encodes input1.txt, input2.txt into output1.txt and outout2.txt and decodes them back into input1_2.txt and input2_2.txt 
*/
void test_EnDecoder_async()
{
	FILE* key1, *key2;
	fopen_s(&key1, "key1.txt", "wb");
	fopen_s(&key2, "key2.txt", "wb");
	for(int i = 0; i < 56; i++)
	{
		int buff = rand();
		int nuls = 0;
		fwrite(&buff, sizeof(int), 1, key1);
		buff = rand();
		fwrite(&buff, sizeof(int), 1, key2);
	}
	fclose(key1);
	fclose(key2);

	test_EnDecoder_async_one_way("input1.txt", "input2.txt", "output1.txt", "outptu2.txt");
	test_EnDecoder_async_one_way("output1.txt", "outptu2.txt", "input1_2.txt", "input2_2.txt");
}


void test_LengthProducer_prepareFiles(vector<const string> names)
{
	int testLengthes[3] = {100, 10, 20};
	char buffer[100];
	memset(buffer, 0, 100);
	for(int i = 0; i < 3; i++)
	{
		FILE* file = fopen(names[i].c_str(), "wb");
		fwrite(buffer, 1, testLengthes[i], file);
		fclose(file);
	}

}


void test_LengthProducer()
{
	const char* names[] =  {"length1.txt", "length2.txt", "length3.txt"};
	vector<const string> v_names;
	for (int i = 0; i< 3; i++)
	{
		v_names.push_back(string(names[i]));
	}
	test_LengthProducer_prepareFiles(v_names);
	LengthProducer producer(&v_names, 60);
	Lengthes nextLengthes;
	do
	{
		nextLengthes = producer.getNextLengthes();
		for(vector<ThreadLengthPair> ::iterator it = nextLengthes.answer.begin(); it < nextLengthes.answer.end(); it++)
		{
			cout << it ->thread << " " << it ->length <<" ";
		}
		cout <<endl;		
	}while(nextLengthes.count > 0);
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




void test_EncodedDataAccessor_genPointers(char* pointers, char* lengthes)
{
	for(char i = 0; i < 3; i++)
	{
		pointers[2*i] = i;
		pointers[2*i+1] = lengthes[i];
	}
}


void test_EncodedDataAccessor_checkData(char* buffer)
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
	vector<unsigned long int> bytes;
	bytes.push_back(100);
	bytes.push_back(100);
	bytes.push_back(100);

	test_EncodedDataAccessor_prepairFiles(names);
	EncodedDataAccessor* dataAccessor = new EncodedDataAccessor(&keys, &names, &bytes);
	dataAccessor ->__testStart();
	char* buffer = new char[306];
	__int8 pointers[6];
	char lengthes[] = {3, 27, 30};
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
	if(output[0] != 300 || output[1] != 3 || output[2] != 1 || output[3] != 0 || output[4] != 1)
	{
		cerr << "Error in EncoderCountpercentage"<< output << endl;
	}
}

void test_LengthGenerator_GetFSObjectSize()
{
	vector<const string> names;
	LengthProducer tester;
	if (tester.__GetFSObjectSize(string("test/value_test1")) != 15)
	{
		log_error("test/value_test1 error");		
	}
	if (tester.__GetFSObjectSize(string("test/value_test2")) != 30)
	{
		log_error("test/value_test2 error");		
	}
	if (tester.__GetFSObjectSize(string("test/value_test3")) != 5)
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
	MapEncoder tester(&keys);
	for(int i = 0; i < keys.size(); i++)
	{
		if(keys[i].compare(tester.threads[i] ->key))
		{
			log_test("MapEncoder_costructor");
			return;
		}
	}
}


void beginTests()
{
	test_EnDecoder();
	test_EnDecoder_async();
	test_LengthProducer();
	test_EncodedDataAccessor();
	test_EncoderCountpercentage();
	test_LengthGenerator_GetFSObjectSize();
	Friendly ::test_formCollectionOfAvailableKeys();
	Friendly ::test_Encoder_keyForThread();
	Friendly ::test_Encoder_generateKeyForThread();
	Friendly ::test_Encoder_renameKey();
	Friendly ::test_MapEncoder_costructor();
	getch();
}