#include "Encoder.h"
#include <vector>
#include <boost\filesystem.hpp>
#include <boost\filesystem\convenience.hpp>
#include "logging.h"
#include <string>
#include "helper.h"
#include <list>
#include "RandomGenerator.h"
#include "EncodedDataAccessor.h"
#include "LengthProducer.h"
using namespace std;
using namespace boost ::filesystem;


Encoder ::Encoder(){};

int Encoder ::Start(vector<const string>* except, const  vector<const string>* only)
{
	return Start(except, only, 150);
};


int Encoder ::Start(vector<const string>* except, const  vector<const string>* only, char size)
{
	data_block = this ->countDataBlock(size);
	if (data_block < 1)
	{
		throw 1;
	}
	if (!only ->empty())
	{
		this ->targets = *only;		
	}
	else
	{
		path thisDirectory = path(".");		
		directory_iterator end_directory;			
		for (directory_iterator it(thisDirectory); it != end_directory; ++it)
		{
			if((extension(it ->path().filename()).compare(".key")) && !vector_contains_string(except, &(it ->path().filename().string())))
			{
				this ->targets.push_back(it ->path().filename().string());				
			}			
		}		
	}
	lengther = LengthProducer(&targets, data_block);
	initialThreadValues = lengther.getAllAvailable();


	int threadNumberItaerator = 0;
	EncoderInteractive asker;
	formCollectionOfAvailableKeys(".");
	for(vector<const string> ::iterator it = targets.begin(); it < targets.end(); it++)
	{
		path key = keyForThread(&*it);
		if(key.empty())
		{
			key = generateKeyForThread(&*it, threadNumberItaerator);
		}
		else
		{
			key = renameKey(&key, &*it, threadNumberItaerator);
		}
		keys.push_back(key);
		verbalKeys.push_back(asker.askForVerbalKey(*it, basename(*it)));
		threadNumberItaerator++;
	}
	dataAccessor = EncodedDataAccessor(&keys, &targets, &initialThreadValues);
	randomer = RandomGenerator(data_block);
	mapper = MapEncoder(&verbalKeys);
	return 0;
};


int Encoder ::countDataBlock(int percentage)
{
	float p = (float)percentage / 100;
	if(p <= 1 || p > 4)
	{
		return 0;
	}
	if (p < 1.012)
	{
		p = 1.012;
	}
	return (int)(769 / (p - 1) / 256);
}



Encoder ::EncoderInteractive ::EncoderInteractive()
{
	filenameForAll = false;
	previostForAll = false;
};

void Encoder ::formCollectionOfAvailableKeys(const char* root)
{
	directory_iterator end;
	for (directory_iterator it(root); it != end; ++it)
	{
		if(!extension(it ->path().filename()).compare(".key"))
		{
			this ->availableKeys.push_back(it ->path());			
		}
	}
};


path Encoder ::keyForThread(const string* threadname)
{
	int length = threadname ->length();
	list<path> ::iterator it = availableKeys.begin();
	list<path> ::iterator end = availableKeys.end();
	while(it != end)
	{
		string availableThreadname = basename(*it);
		int lastPos = availableThreadname.find_last_of("_");
		if (lastPos == availableThreadname.npos || lastPos < 1)
		{
			lastPos = availableThreadname.length();
		}
		availableThreadname = availableThreadname.substr(0, lastPos);
		if(!availableThreadname.compare(*threadname))
		{
			path toReturn(*it);
			availableKeys.erase(it);
			return toReturn;
		}
		else
		{
			it++;
		}
	}
	return path();
};



string Encoder ::EncoderInteractive ::askForVerbalKey(const string target, const string proposal)
{
	if(filenameForAll)
	{
		return proposal;
	}
	else 
	{
		if(previostForAll)
		{
			return previostAnswer;
		}
	}
	cout << "What will be the password for " << target << "? (live blank for password" << proposal << ",  or !p to select previost)" <<endl;
	cout << "Enter !f to select filenames as password for all future files" << endl;
	if (!previostAnswer.empty())
	{
		cout << "Enter !p to select \'" << previostAnswer << "\'as password for all files" << endl;
	}
	string answer;
	cin >> answer;
	if (answer.length() == 0)
	{
		previostAnswer = proposal;
		return previostAnswer;		
	}
	if(!answer.compare("!f"))
	{
		filenameForAll = true;
		previostAnswer = proposal;		
	}
	else
	{
		if(!answer.compare("!p") && !previostAnswer.empty())
		{
			previostForAll = true;			
		}
		else
		{
			previostAnswer = answer;
		}
	}
	return previostAnswer;

};


path Encoder:: generateKeyForThread(const string* name, unsigned char number)
{
	char buffer[112];
	RandomGenerator random;
	random.getNewKey(buffer);
	string filename = *name;
	filename.append("_");
	char str_threadnumber[4];
	itoa(number, str_threadnumber, 10);
	filename.append(str_threadnumber);
	filename.append(".key");
	FILE* file = fopen(filename.c_str(), "wb");
	fwrite(buffer, 1, 112, file);
	fclose(file);
	return path(filename);
};


path Encoder ::renameKey(path* oldPath, const string* pureName, unsigned char thread)
{
	string newName = *pureName;
	newName.append("_");
	char str_thread[4];
	itoa(thread, str_thread, 10);
	newName.append(str_thread);
	newName.append(".key");
	path newPath = oldPath ->parent_path();
	newPath /= newName;
	if(exists(newPath))
	{
		remove(newPath);		
	}
	rename(*oldPath, newPath);
	return newPath;
};


