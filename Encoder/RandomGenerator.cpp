#include "RandomGenerator.h"
#include <algorithm>

vector<int> RandomGenerator ::randomVector(int n)
{
	vector<int> toReturn;	
	for (int i=0; i<n; ++i) toReturn.push_back(i);
	random_shuffle ( toReturn.begin(), toReturn.end() );
	return toReturn;
}
