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
#include "Constants.h"
using namespace std;
using namespace boost ::filesystem;


Encoder ::Encoder(){};

int Encoder ::start(vector<const string>* except, const  vector<const string>* only)
{
	return start(except, only, 150);
};


int Encoder ::start(vector<const string>* except, const  vector<const string>* only, int size)
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
	lengther.create(&targets, data_block);
	

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
		threadNumberItaerator++;
	}
	dataAccessor.create(&keys, &targets);
	dataAccessor.Start();
	for(vector<const string> ::iterator it = targets.begin(); it < targets.end(); it++)
	{
		verbalKeys.push_back(asker.askForVerbalKey(*it, basename(*it)));
	}	
	randomer.create(data_block);
	mapper.create(&verbalKeys);
	BYTE threadsCount = targets.size();
	fopen_s(&outputFile, "some_name.extn", "wb");
	fwrite(&threadsCount, 1, 1, outputFile);
	runEncodingCycle(threadsCount);
	fclose(outputFile);
	return 0;
};

void Encoder ::runEncodingCycle(BYTE threadCount)
{
	Lengthes whatShouldIRead = lengther.getNextLengthes();
	BYTE* dataBuffer = new BYTE[data_block * BLOCK_COUNT];
	do
	{
		BYTE pointers[BLOCK_COUNT*2];
		BYTE prevMapsLength = 0;
		for (int i = 0; i < threadCount; i++)
		{
			pointers[2*i+1] = whatShouldIRead.answer[i].length;
			pointers[2*i] = pointers[2*i+1] ? prevMapsLength : 0;
			prevMapsLength += pointers[2*i+1];
		}
		dataAccessor.getData(dataBuffer, pointers, data_block);
		randomer.genNextRandom();
		randomer.shuffle(dataBuffer);
		char encodedPointers[BLOCK_COUNT*2];
		char encodedMap[BLOCK_COUNT];
		memcpy(encodedPointers, pointers, BLOCK_COUNT*2);
		memcpy(encodedMap, randomer.getMapPointer(), BLOCK_COUNT);
		mapper.encodePointersAndMap(encodedPointers, encodedMap);
		char newMap = 1;
		fwrite(&newMap, 1, 1, outputFile);
		fwrite(encodedPointers, 2, MAX_FILE_COUNT, outputFile);
		fwrite(encodedMap, BLOCK_COUNT, 1, outputFile);
		fwrite(dataBuffer, BLOCK_COUNT, data_block, outputFile);	
		newMap = 0;
		while(--whatShouldIRead.count > 0)
		{
			dataAccessor.getData(dataBuffer, pointers, data_block);
			randomer.shuffle(dataBuffer);
			fwrite(&newMap, 1, 1, outputFile);
			fwrite(dataBuffer, BLOCK_COUNT, data_block, outputFile);
		}
		tellPercentageDone(lengther.getProcessedPart());
		whatShouldIRead = lengther.getNextLengthes();
	}while(whatShouldIRead.count > 0);
	delete[] dataBuffer; 

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
	cout << "What will be the password for " << target << "? (enter !y for password " << proposal  << ')'<<endl;
	cout << "Enter !f to select filenames as password for all further files" << endl;
	if (!previostAnswer.empty())
	{
		cout << "Enter !p to select \'" << previostAnswer << "\'as password for all files" << endl;
	}
	cout << "!e" << " to exit encoding." << endl;
	string answer;
	cin >> answer;
	if (!answer.compare("!y"))
	{
		previostAnswer = proposal;		
	}
	else 
	{
		if(!answer.compare("!f"))
		{
			filenameForAll = true;
			previostAnswer = proposal;		
		}
		else
		{
			if(!answer.compare("!e"))
			{
				throw 1;
			}
			if(!answer.compare("!p") && !previostAnswer.empty())
			{
				previostForAll = true;			
			}
			else
			{
				if(!answer.compare("!n"))
				{
					previostAnswer = string();
				}
				else
				{
					previostAnswer = answer;
				}
			}
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
	if(exists(newPath) && newPath != *oldPath)
	{
		remove(newPath);		
	}
	rename(*oldPath, newPath);
	return newPath;
};

void Encoder ::tellPercentageDone(float partDone)
{
	cout << 100 - int(partDone * 100) << " %" << endl;
}


Encoder ::~Encoder()
{}
