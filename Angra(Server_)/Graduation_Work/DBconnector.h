#pragma once
#include<string.h>
#include<stdio.h>
#include<sql.h>
#include<sqlext.h>


class DBconnector
{
private:
	/*SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;
	SQLINTEGER num;
	SQLRETURN retcode;

	SQLWCHAR sCustID[32];
	SQLLEN cbName = 0, cbCustID = 0, cbPhone = 0;*/

public:
	//bool Initialize();
	//void DBconnect();
	DBconnector();
	~DBconnector();
};

