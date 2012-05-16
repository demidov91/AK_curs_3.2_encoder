#pragma once
#include <vector>
using namespace std;

class EncodedDataAccessor
{
private:
	int fileCount;
	vector<char*> files;

public:




	/**
	Number of files and names of files.
	*/
	EncodedDataAccessor(int, char**);
	~EncodedDataAccessor(void);
};

