#pragma once
#include <vector>
using namespace std;


class RandomGenerator
	{
	public:
		/**
		Writes 112 random bytes into buffer.
		*/
		void getNewKey(char* buffer);	
		/**
		Generates shuffled vector of length n. 
		*/
		vector<int> randomVector(int n);
		
	};

