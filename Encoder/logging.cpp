#include "logging.h"
#include <iostream>
using namespace std;

void log_error(void* args)
{
	cout << args << endl;
}