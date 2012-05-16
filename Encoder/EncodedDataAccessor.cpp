#include "EncodedDataAccessor.h"
#include <string.h>


EncodedDataAccessor::EncodedDataAccessor(int n, char** files)
{
	this ->fileCount = n;
	this ->files.resize(n, NULL);
	for(int i = 0; i < n; i++) strcpy(this ->files[i], files[i]);
}






EncodedDataAccessor::~EncodedDataAccessor(void)
{
}
