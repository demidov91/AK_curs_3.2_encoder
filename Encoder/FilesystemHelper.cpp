#include "FilesystemHelper.h"
#include <boost\filesystem.hpp>
#include"Constants.h"
using namespace std;
using namespace boost ::filesystem;


unsigned long int GetFSObjectSize(string* root)
{
	unsigned long int answer = BYTES_FOR_FSOBJECT + BYTES_FOR_FSOBJECTNAME_LENGTH + path(*root).filename().string().length();
	if (!is_directory(*root))
	{
		return answer + file_size(*root);
	}
	recursive_directory_iterator end;	
	for(recursive_directory_iterator it = recursive_directory_iterator(*root); it != end; it++)
	{
		if(!is_directory(*it))
		{
			answer += BYTES_FOR_FSOBJECT + BYTES_FOR_FSOBJECTNAME_LENGTH + it ->path().filename().string().length() +  file_size(*it);
		}
	}
	return answer;
};