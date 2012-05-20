#include "logging.h"
#include <iostream>
using namespace std;

void log_error(void* args)
{
	cout << (char*)args << endl;
}

void log_test(void* args)
{
	cout << "TEST: " << (const char*)args << endl;
}