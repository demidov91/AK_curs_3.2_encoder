#include "tests.h"
#include <iostream>
#include "main.h"
#include <string>
#include <vector>
#include "Encoder.h"
#include "Decode.h"
using namespace std;

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		cerr << NOT_ENOUGH_PARAMETERS << endl;
		return 1;
	}
		
	if (!strcmp(argv[1], "/e"))
	{
		int size;
		vector<const char*> except;
		vector<const char*> only;
		if(argc == 3)
		{
			cerr << WRONG_USAGE << argv[2] << endl;
			return 1;
		}
		if(argc > 3)
		{
			int positionForOnlyOrExcept = 2;
			if(!strcmp(argv[2], "/size"))
			{
				size = atoi(argv[3]);
				if (size > MAX_ARG_SIZE || size < MIN_ARG_SIZE)
				{
					cerr << WRONG_ARG_SIZE << endl;
					return 1;
				}
				positionForOnlyOrExcept += 2;
			}
			if (argc > positionForOnlyOrExcept + 1)
			{
				if(!strcmp(argv[positionForOnlyOrExcept], "/only"))
				{
					for(int j = positionForOnlyOrExcept+1; j < argc; j++)
					{
						only.push_back(argv[j]);
					}
				}
				else
				{
					if(!strcmp(argv[positionForOnlyOrExcept], "/except"))
					{
						for(int j = positionForOnlyOrExcept+1; j < argc; j++)
						{
							except.push_back(argv[j]);
						}
					}
					else
					{
						cerr << WRONG_USAGE << argv[positionForOnlyOrExcept] << endl;
						return 1;
					}
				}
			}
		}
		Encoder* processor = new Encoder();
		int numberOfFiles = processor ->Start(&except, &only, size);
		cout << ENCODED_SUCCESSFULLY << endl;
		cout << numberOfFiles << " files encoded." << endl;
		delete processor;
	}
	else
	{
		if (!strcmp(argv[1], "/d"))
		{
			if(argc < 4)
			{
				cerr << WRONG_ARG_D << endl;
				return 1;
			}
			char* str_threadNumber = strrchr(argv[3], '_');
			if (!str_threadNumber || !(*(str_threadNumber+1)))
			{
				cerr << WRONG_ARG_D << endl;
				return 1;
			}
			int int_threadNumber = atoi(str_threadNumber+1);
			if(!int_threadNumber && str_threadNumber[0] != '0')
			{
				cerr << WRONG_ARG_D << endl;
				return 1;
			}
			*str_threadNumber = 0;
			Decode* decoder = new Decode();
			int decoded = decoder ->Start(argv[2], argv[3], int_threadNumber);
			cout << ENCODED_SUCCESSFULLY << endl;
			cout << decoded << " files encoded." << endl;
			delete decoder;
		}
		else
		{
			cerr << WRONG_USAGE << argv[1] << endl;
		}
	}
	return 0;
}