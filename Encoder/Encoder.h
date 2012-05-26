#pragma once

#include <vector>
using namespace std;


class Encoder
{

public:
	Encoder();

	/**
	Enodes all files in working directory except files in 'except' vector or only in 'only' vector.
	Parameter size supposed to be used like a percentage of value of output file in comparison to input sum file values.
	Returns number of encoded files.
	*/
	int Start(vector<const char*>* except, const  vector<const char*>* only, int size);

};