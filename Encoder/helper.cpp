#include "helper.h"
#include <string>
using namespace std;

bool vector_contains_string(vector<const string> *v, const string* s)
{
	for(vector<const string> ::iterator it =  v ->begin(); it < v ->end(); it++)
	{
		if (!s ->compare(*it))
		{
			return true;
		}	
	}
	return false;
};