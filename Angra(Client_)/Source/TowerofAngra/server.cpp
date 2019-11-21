// Fill out your copyright notice in the Description page of Project Settings.
#pragma warning(disable : 4996)

#include "server.h"
#include <sstream>
#include "Runtime/Core/Public/GenericPlatform/GenericPlatformAffinity.h"
#include "Runtime/Core/Public/HAL/Runnable.h"
#include "Runtime/Core/Public/Async/Async.h"

#include "RunnableThread.h"
#include <algorithm>
#include <string>
#include "CharacterPlayerController.h"
#include "Async.h"

server::server() :StopTaskCounter(0)
{
}


server::~server()
{
	delete Thread;
	Thread = nullptr;

	closesocket(Sock);
	WSACleanup();
}

bool server::InitSock()
{
	WSADATA wsaData;
	// 윈속 버전을 2.2로 초기화
	int retval = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (retval != 0)
	{
		return false;
	}

	// TCP 소켓 생성	
	Sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (Sock == INVALID_SOCKET)
	{
		return false;
	}

	return true;
}
bool server::Connect(const char* pszIP, int nPort)
{
	// 접속할 서버 정보를 저장할 구조체
	SOCKADDR_IN stServerAddr;

	stServerAddr.sin_family = AF_INET;
	// 접속할 서버 포트 및 IP
	stServerAddr.sin_port = htons(nPort);
	stServerAddr.sin_addr.s_addr = inet_addr(pszIP);

	int nRet = connect(Sock, (sockaddr*)&stServerAddr, sizeof(sockaddr));
	if (nRet == SOCKET_ERROR)
	{
		return false;
	}

	return true;
}
void server::EnrollCharacterInfo(cPlayer& info)
{
	stringstream SendStream;

	SendStream << PacketType::ENROLL_CHARACTER << endl;;
	SendStream << info;

	int nSendLen = send
	(
		Sock,
		(CHAR*)SendStream.str().c_str(),
		SendStream.str().length(),
		0
	);

	if (nSendLen == -1)
	{
		return;
	}
}

// 캐릭터 동기화
void server::SendPlayer(cPlayer& info)
{
	// 캐릭터 정보 직렬화

	stringstream SendStream;
	// 요청 종류
	SendStream << PacketType::SEND_CHARACTER << endl;
	SendStream << info;

	// 캐릭터 정보 전송
	int nSendLen = send
	(
		Sock,
		(CHAR*)SendStream.str().c_str(),
		SendStream.str().length(),
		0
	);

	if (nSendLen == -1)
	{
		return;
	}
}

// 캐릭터 로그아웃
void server::LogoutCharacter(int SessionId)
{
	stringstream SendStream;
	SendStream << PacketType::LOGOUT_CHARACTER << endl;
	SendStream << SessionId << endl;

	int nSendLen = send
	(
		Sock,
		(CHAR*)SendStream.str().c_str(),
		SendStream.str().length(),
		0
	);

	if (nSendLen == -1)
	{
		return;
	}

	closesocket(Sock);
	WSACleanup();
}
// 스레드 시작 및 종료
bool server::StartListen()
{
	// 스레드 시작
	if (Thread != nullptr)
	{
		cout << "Thread != nullptr" << endl;
		return false;
	}

	Thread = FRunnableThread::Create(this, TEXT("serversocket"), 0, TPri_BelowNormal);

	return (Thread != nullptr);
}
void server::CloseSocket()
{
	closesocket(Sock);
	WSACleanup();
}
void server::StopListen()
{
	Stop();
	Thread->WaitForCompletion();
	Thread->Kill();
	delete Thread;
	Thread = nullptr;
	StopTaskCounter.Reset();
}

cPlayerInfo* server::RecvCharacterInfo(stringstream& RecvStream)
{
	RecvStream >> playerinfo;
	return &playerinfo;
}

//FRunnable override 함수
bool server::Init()
{
	return true;

}

uint32 server::Run()
{
	FPlatformProcess::Sleep(0.03);
	while (StopTaskCounter.GetValue() == 0)
	{
		stringstream RecvStream;
		int PacketType;
		int nRecvLen = recv(Sock, (CHAR*)&recvBuffer, SOCKET_BUF_SIZE, 0);
		if (nRecvLen > 0)
		{
			RecvStream << recvBuffer;
			RecvStream >> PacketType;

			switch (PacketType)
			{
			case PacketType::RECV_CHARACTER:
			{
				Controller->RecvWorldInfo(RecvCharacterInfo(RecvStream));
			}
			break;
			case PacketType::ENTER_NEW_PLAYER:
			{
				Controller->RecvNewPlayer(RecvNewPlayer(RecvStream));
			}
			break;
			case PacketType::SPAWN_MONSTER:
			{

				Controller->RecvSpawnMonster(RecvMonster(RecvStream));
			}
			break;

			case PacketType::NEXT_LEVEL_STAGE_SPAWN_MONSTER:
			{


				Controller->NextStageRecvSpawnMonster(NextStageRecvMonster(RecvStream));
			}
			break;
			case PacketType::SYNCRO_MONSTER:
			{
				Controller->RecvNewMonster(RecvMonsterSet(RecvStream));
			}
			break;
			case PacketType::NEXT_LEVEL_STAGE_SYNCRO_MONSTER:
			{
				Controller->NextStageRecvNewMonster(NextStageRecvMonsterSet(RecvStream));
			}
			break;
			case PacketType::DESTROY_MONSTER:
			{
				Controller->RecvDestroyMonster(RecvMonster(RecvStream));
			}
			break;
			case PacketType::NEXT_LEVEL_STAGE_DESTROY_MONSTER:
			{
				Controller->NextStageRecvDestroyMonster(NextStageRecvMonster(RecvStream));
			}
			break;
			default:
				break;
			}
		}
	}
	return 0;
}
void server::Stop()
{
	StopTaskCounter.Increment();
}
void server::Exit()
{

}

MonsterSet* server::RecvMonsterSet(stringstream& RecvStream)
{
	RecvStream >> MonsterSetInfo;
	return &MonsterSetInfo;
}
Monster* server::RecvMonster(stringstream& RecvStream)
{
	RecvStream >> MonsterInfo;
	return &MonsterInfo;
}


//====================================================================================================================다음 스테이지
NextStageMonsterSet* server::NextStageRecvMonsterSet(stringstream& RecvStream)
{
	RecvStream >> NextStageMonsterSetInfo;
	return &NextStageMonsterSetInfo;
}
NextStageMonster* server::NextStageRecvMonster(stringstream& RecvStream)
{
	RecvStream >> NextStageMonsterInfo;
	return &NextStageMonsterInfo;
}
//====================================================================================================================다음 스테이지

cPlayer *server::RecvNewPlayer(stringstream & RecvStream)
{
	RecvStream >> NewPlayer;
	return &NewPlayer;
}

void server::SetPlayerController(ACharacterPlayerController* PlayerController)
{
	if (PlayerController)
	{
		Controller = PlayerController;
	}
}

void server::NextStageHitMonster(const int & MonsterID, const float& Hp, const float& damage, const bool& isDie)
{
	// 서버에게 데미지를 준 몬스터 정보 전송
	stringstream SendStream;
	SendStream << PacketType::HIT_NEXT_STAGE_MONSTER << endl;
	SendStream << MonsterID << endl;
	SendStream << Hp << endl;
	SendStream << damage << endl;
	SendStream << isDie << endl;

	int nSendLen = send
	(
		Sock,
		(CHAR*)SendStream.str().c_str(),
		SendStream.str().length(),
		0
	);
}
//=====================================================2019-08-03
void server::HitMonster(const int & MonsterID, const float& Hp, const float& damage, const bool& isDie)
{
	// 서버에게 데미지를 준 몬스터 정보 전송
	stringstream SendStream;
	SendStream << PacketType::HIT_MONSTER << endl;
	SendStream << MonsterID << endl;
	SendStream << Hp << endl;
	SendStream << damage << endl;
	SendStream << isDie << endl;

	int nSendLen = send
	(
		Sock,
		(CHAR*)SendStream.str().c_str(),
		SendStream.str().length(),
		0
	);
}
void server::HitPlayer(const int& SessionId)
{
	// 서버에게 데미지를 준 캐릭터 정보 전송
	stringstream SendStream;
	SendStream << PacketType::HIT_PLAYER << endl;
	SendStream << SessionId << endl;

	int nSendLen = send
	(
		Sock,
		(CHAR*)SendStream.str().c_str(),
		SendStream.str().length(),
		0
	);
}
//=====================================================2019-08-03