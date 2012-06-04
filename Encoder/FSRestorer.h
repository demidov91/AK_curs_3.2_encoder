#pragma once
#include <Windows.h>


class FSRestorer
{
public:
	HANDLE pipe;
	FSRestorer(void);
	bool isAlive();
	~FSRestorer(void);
};

