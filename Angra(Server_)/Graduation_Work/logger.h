#pragma once
#include"pch.h"
#include"singletonBase.h"
#include<fstream>

class LogBase
{
public:
	LogBase() {}
	virtual ~LogBase() {}
	virtual void initialize() {}
	virtual void unInitialize() {}
	virtual void log(WCHAR * logStr) = 0;
};

//-------------------------------------------------------------------
class LogPrintf : public LogBase
{
public:
	LogPrintf();
	void log(WCHAR * logStr);
};

//-------------------------------------------------------------------
class LogFile : public LogBase
{
private:
	std::wfstream	fs_;
	wstr_t			fileName_;

public:
//	LogFile(xml_t * config);
	virtual ~LogFile();

	void initialize() {}
	void initialize(WCHAR * logFileName);
	void log(WCHAR * logStr);

};

class LogWriter
{
private:
	LogBase		*base_;
	wstr_t		prefix_;
public:
	LogWriter();
	virtual ~LogWriter();

	void setLogger(LogBase *base, const WCHAR* logPrefix);
	LogBase* logger();

	void log(WCHAR *fmt, ...);
	void log(WCHAR* fmt, va_list args);
};
typedef LogWriter* LogWriterPtr;


class SystemLog : public singletonBase<SystemLog>
{
private:
	LogWriter	logWrite_;
public:
	SystemLog();
	virtual ~SystemLog();
//	void initialize(xml_t* config);
	void log(WCHAR* fmt, ...);
};