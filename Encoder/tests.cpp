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


void test_LengthProducer_prepareFiles(const char** names)
{
	int testLengthes[3] = {100, 10, 20};
	char buffer[100];
	memset(buffer, 0, 100);
	for(int i = 0; i < 3; i++)
	{
		FILE* file = fopen(names[i], "wb");
		fwrite(buffer, 1, testLengthes[i], file);
		fclose(file);
	}

}


void test_LengthProducer()
{
	const char* names[] =  {"length1.txt", "length2.txt", "length3.txt"};
	test_LengthProducer_prepareFiles(names);
	LengthProducer producer(3, names, 60);
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


void beginTests()
{
	test_EnDecoder();
	test_EnDecoder_async();
	test_LengthProducer();
	getch();
}