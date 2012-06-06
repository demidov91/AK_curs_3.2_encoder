#include "tests.h"
#include <iostream>
#include "main.h"
#include <string>
#include <vector>
#include "Encoder.h"
#include "Decode.h"
#include <boost\filesystem.hpp>
using namespace std;
using namespace boost ::filesystem;

void initBoost()
{
	path bfinml(".");
}


int main(int argc, char** argv)
{
	initBoost();
	//beginTests();
	//return 0;
	if (argc < 2)
	{
		cerr << NOT_ENOUGH_PARAMETERS << endl;
		return 1;
	}
		
	if (!strcmp(argv[1], "/e"))
	{
		int size = 0;
		vector<const string> except;
		vector<const string> only;
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
						only.push_back(string(argv[j]));
					}
				}
				else
				{
					if(!strcmp(argv[positionForOnlyOrExcept], "/except"))
					{
						for(int j = positionForOnlyOrExcept+1; j < argc; j++)
						{
							except.push_back(string(argv[j]));
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
		try{
		int numberOfFiles = size!=0 ? processor ->start(&except, &only, size) : processor ->start(&except, &only);
		}
		catch(int error)
		{
			delete processor;
			return 1;
		}
		cout << ENCODED_SUCCESSFULLY << endl;
		delete processor;
	}
	else
	{
		if (!strcmp(argv[1], "/d"))
		{
			if(argc < 5)
			{
				cerr << WRONG_ARG_D << endl;
				return 1;
			}
			char* str_threadNumber = strrchr(argv[4], '_');
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
			try{
				int decoded = decoder ->start(argv[2], argv[3], argv[4], int_threadNumber);
			}
			catch(int error)
			{
				delete decoder;
				return 1;
			}
			cout << ENCODED_SUCCESSFULLY << endl;
			delete decoder;
		}
		else
		{
			cerr << WRONG_USAGE << argv[1] << endl;
		}
	}
	return 0;
}