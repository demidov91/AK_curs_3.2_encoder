#include "FilesystemHelper.h"
#include <boost\filesystem.hpp>
using namespace std;
using namespace boost ::filesystem;

unsigned long int GetFSObjectSize(string* root)
{
	if (!is_directory(*root))
	{
		return file_size(*root);
	}
	recursive_directory_iterator end;
	unsigned long int answer = 0;
	for(recursive_directory_iterator it = recursive_directory_iterator(*root); it != end; it++)
	{
		if(!is_directory(*it))
		{
			answer += file_size(*it);
		}
	}
	return answer;
};