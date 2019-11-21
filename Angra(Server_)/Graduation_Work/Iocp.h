#pragma once
#include"pch.h"


class iocp : public singletonBase<iocp>, public Server
{
private:
	int ThreadCount;


public:
	iocp();
	~iocp();

public:

};

