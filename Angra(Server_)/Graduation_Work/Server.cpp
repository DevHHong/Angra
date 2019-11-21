#pragma once
#include "pch.h"
#include "Server.h"
#include<process.h>
#include<sstream>
#include "Monster.h"


#pragma warning (disable : 4996)

map<int, SOCKET>	Server::SessionSocket;
cPlayerInfo			Server::playerinfo;
MonsterSet			Server::Monsterinfo;
NextStageMonsterSet Server::NextStageMonsterinfo;
int					Server::CurrentStageLevel;


unsigned int WINAPI CallWorkerThread(LPVOID p)
{
	Server* pOverlappedEvent = (Server*)p;
	pOverlappedEvent->WorkerThreadFunc();
	return 0;
}

unsigned int WINAPI CallMonsterWorkerThread(LPVOID p)
{
	Server* pOverlappedEvent = (Server*)p;
	pOverlappedEvent->MonsterThread();
	return 0;
}

unsigned int WINAPI CallNextStageMonsterWorkerThread(LPVOID p)
{
	Server* pOverlappedEvent = (Server*)p;
	pOverlappedEvent->NextStageMonsterThread();
	return 0;
}

Server::Server()
{
	WorkerThread = true;
	Accept = true;
	CurrentStageLevel = -1;

}

Server::~Server()
{
	WSACleanup();
	if (SocketInfo)
	{
		delete[] SocketInfo;
		SocketInfo = NULL;
	}

	if (WorkerHandle)
	{
		delete[] WorkerHandle;
		WorkerHandle = NULL;
	}
}

bool Server::Initialize()
{
	WSADATA wsaData;
	int nResult;
	nResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (nResult != 0)
	{
		printf("[ERROR] winsock 초기화 실패\n");
		return false;
	}


	ListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (ListenSocket == INVALID_SOCKET)
	{
		printf("[ERROR] 소켓 생성 실패\n");
		return false;
	}

	int option = TRUE; 

	setsockopt(ListenSocket,             
		IPPROTO_TCP,         
		TCP_NODELAY,          
		(const char*)&option, 
		sizeof(option));      

	SOCKADDR_IN serverAddr;

	ZeroMemory(&serverAddr, sizeof(SOCKADDR_IN));

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVERPORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	// 소켓 설정
	nResult = ::bind(ListenSocket, (struct sockaddr*)&serverAddr, sizeof(SOCKADDR_IN));
	if (nResult == SOCKET_ERROR)
	{
		printf("[ERROR] bind 실패\n");
		closesocket(ListenSocket);
		WSACleanup();
		return false;
	}

	nResult = listen(ListenSocket, SOMAXCONN);
	if (nResult == SOCKET_ERROR)
	{
		printf("[ERROR] listen 실패\n");
		closesocket(ListenSocket);
		WSACleanup();
		return false;
	}

	return true;
}

void Server::StartServer()
{
    CreateMonsterWorkerThread();
	NextStageCreateMonsterWorkerThread();

	int nResult;
	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(SOCKADDR_IN);
	SOCKET clientSocket;
	DWORD recvBytes;
	DWORD flags;
	Iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0,NULL,0);

	if (!CreateWorkerThread())
		return;

	auto tp = std::chrono::system_clock::now();
	auto t = std::chrono::system_clock::to_time_t(tp);
	cout << "------------------------------------------------------------------" << endl;
	cout << "\t  서버 시작 시간 - " << ctime(&t);
	cout << "------------------------------------------------------------------" << endl;


	// 클라이언트 접속을 받음
	while (Accept)
	{
		clientSocket = WSAAccept
		(
			ListenSocket,
			(struct sockaddr *)&clientAddr,
			&addrLen,
			NULL, 
			NULL
		);

		if (clientSocket == INVALID_SOCKET)
		{
			return;
		}

		// 접속한 클라이언트 정보 출력
//		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
//			inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

		SocketInfo = new SOCKETINFO();
		SocketInfo->socket = clientSocket;
		SocketInfo->recvBytes = 0;
		SocketInfo->sendBytes = 0;
		SocketInfo->dataBuf.len = SOCKET_BUF_SIZE;
		SocketInfo->dataBuf.buf = SocketInfo->messageBuffer;
		flags = 0;

		Iocp = CreateIoCompletionPort(
			reinterpret_cast<HANDLE>(clientSocket), Iocp, (DWORD)SocketInfo, 0
		);

		// 중첩 소켓을 지정하고 완료시 실행될 함수를 넘겨줌
		nResult = WSARecv
		(
			SocketInfo->socket,
			&SocketInfo->dataBuf,
			1,
			&recvBytes,
			&flags,
			&(SocketInfo->overlapped),
			NULL
		);

		if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
		{
			printf("[ERROR] IO Pending 실패 : %d", WSAGetLastError());
			return;
		}
	}

}



void Server::WorkerThreadFunc()
{
	BOOL	bResult;
	DWORD	recvBytes;
	DWORD	sendBytes;
	SOCKETINFO *	pCompletionKey;	
	SOCKETINFO *	pSocketInfo;

	DWORD	dwFlags = 0;

	while (WorkerThread)
	{
		bResult = GetQueuedCompletionStatus
		(
			Iocp,
			&recvBytes,				
			(PULONG_PTR)&pCompletionKey,	
			(LPOVERLAPPED *)&pSocketInfo,			
			INFINITE				
		);

		if (!bResult && recvBytes == 0)
		{
			printf("socket(%d) 접속 끊김\n", pSocketInfo->socket);
			closesocket(pSocketInfo->socket);
			free(pSocketInfo);
			continue;
		}

		pSocketInfo->dataBuf.len = recvBytes;

		if (recvBytes == 0)
		{
			cout << "recvByte == 0" << endl;
			closesocket(pSocketInfo->socket);
			free(pSocketInfo);
			continue;
		}

		else
		{
			int PacketType;
			stringstream RecvStream;


			RecvStream << pSocketInfo->dataBuf.buf;
			RecvStream >> PacketType;


			switch (PacketType)
			{
			case PacketType::ENROLL_CHARACTER:
			{
				EnrollCharacter(RecvStream, pSocketInfo);
			}
			break;
			case PacketType::SEND_CHARACTER:
			{
				SyncCharacters(RecvStream, pSocketInfo);
			}
			break;
			case PacketType::LOGOUT_CHARACTER:
			{
				LogoutCharacter(RecvStream, pSocketInfo);
			}
			break;
			case PacketType::HIT_MONSTER:
			{
				HitMonster(RecvStream, pSocketInfo);
			}
			break;
			case PacketType::HIT_NEXT_STAGE_MONSTER:
			{
				NextStageHitMonster(RecvStream, pSocketInfo);
			}
			break;
			case PacketType::HIT_PLAYER:
			{
				HitPlayer(RecvStream, pSocketInfo);
			}
			break;

			default:
				break;
			
			}
			Recv(pSocketInfo);
		}
	}
}

void Server::HitPlayer(stringstream& RecvStream, SOCKETINFO* Socketinfo)
{
	int DamagedSessionId;
	RecvStream >> DamagedSessionId;

	playerinfo.players[DamagedSessionId].HP -= 10;		
	if (playerinfo.players[DamagedSessionId].HP < 0)
	{
		playerinfo.players[DamagedSessionId].IsAliving = false;
	}

	WriteCharactersInfoToSocket(Socketinfo);
	Send(Socketinfo);
}

void Server::HitMonster(stringstream& RecvStream, SOCKETINFO* Socketinfo)
{
	int MonsterId;
	float Hp;
	float Damage;
	bool isDie;
	RecvStream >> MonsterId;

	Monsterinfo.monsters[MonsterId].RecvDamage(40.0f);

	if (!Monsterinfo.monsters[MonsterId].IsAlive())
	{
		stringstream SendStream;
		SendStream << PacketType::DESTROY_MONSTER << endl;
		SendStream << Monsterinfo.monsters[MonsterId] << endl;

		Broadcast(SendStream);

		Monsterinfo.monsters.erase(MonsterId);
	}
}
void Server::NextStageHitMonster(stringstream& RecvStream, SOCKETINFO* Socketinfo)
{
	int MonsterId;
	float Hp;
	float Damage;
	bool isDie;
	RecvStream >> MonsterId;

	NextStageMonsterinfo.monsters[MonsterId].RecvDamage(40.0f);
	if (!NextStageMonsterinfo.monsters[MonsterId].IsAlive())
	{
		stringstream SendStream;
		SendStream << PacketType::DESTROY_MONSTER << endl;
		SendStream << NextStageMonsterinfo.monsters[MonsterId] << endl;

		Broadcast(SendStream);

		NextStageMonsterinfo.monsters.erase(MonsterId);
	}
}

void Server::Send(SOCKETINFO* Socketinfo)
{
	int nResult;
	DWORD	sendBytes;
	DWORD	dwFlags	  = 0;

	nResult = WSASend
	(
		Socketinfo->socket,
		&(Socketinfo->dataBuf),
		1,
		&sendBytes,
		dwFlags,
		NULL,
		NULL
	);

	if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
	{
		printf("[ERROR] WSASend 실패 : \n", WSAGetLastError());
	}
}

void Server::Recv(SOCKETINFO* Socketinfo)
{
	int nResult;
	// DWORD	sendBytes;
	DWORD	dwFlags = 0;

	ZeroMemory(&(Socketinfo->overlapped), sizeof(OVERLAPPED));
	ZeroMemory(Socketinfo->messageBuffer, SOCKET_BUF_SIZE);
	Socketinfo->dataBuf.len = SOCKET_BUF_SIZE;
	Socketinfo->dataBuf.buf = Socketinfo->messageBuffer;
	Socketinfo->recvBytes = 0;
	Socketinfo->sendBytes = 0;

	dwFlags = 0;

	nResult = WSARecv
	(
		Socketinfo->socket,
		&(Socketinfo->dataBuf),
		1,
		(LPDWORD)&Socketinfo,
		&dwFlags,
		(LPWSAOVERLAPPED)&(Socketinfo->overlapped),
		NULL
	);

	if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
	{
		printf("[ERROR] WSARecv 실패 : ", WSAGetLastError());
	}
}
void Server::EnrollCharacter(stringstream &RecvStream, SOCKETINFO *SocketInfo)
{
	cPlayer info;
	RecvStream >> info;
	//printf("%d번 캐릭터 등록 - X :%f, Y :%f, Z :%f, Yaw :%f, Pitch :%f, Roll :%f\n",
	//	info.ClientID, info.X, info.Y, info.Z, info.Yaw, info.Pitch, info.Roll);

	printf("%d번 케릭터", info.ClientID);
	cPlayer* pinfo = &playerinfo.players[info.ClientID];
			
	pinfo->ClientID = info.ClientID;
	pinfo->X = info.X;
	pinfo->Y = info.Y;
	pinfo->Z = info.Z;

	// 캐릭터의 속도를 저장
	pinfo->VX = info.VX;
	pinfo->VY = info.VY;
	pinfo->VZ = info.VZ;

	// 캐릭터의 회전값을 저장
	pinfo->Yaw = info.Yaw;
	pinfo->Pitch = info.Pitch;
	pinfo->Roll = info.Roll;

	pinfo->HP = info.HP;

	pinfo->SkillType = info.SkillType;
	pinfo->IsSkilling = info.IsSkilling;
	pinfo->IsAttacking = info.IsAttacking;
	pinfo->clientPlayerType = info.clientPlayerType;
	CurrentStageLevel = info.StageLevel;

	SessionSocket[info.ClientID] = SocketInfo->socket;
	BroadcastNewPlayer(info);
}

void Server::SyncCharacters(stringstream& RecvStream, SOCKETINFO* Socketinfo)
{
	cPlayer info;
	RecvStream >> info;
	cPlayer * pinfo = &playerinfo.players[info.ClientID];
	// 캐릭터의 위치를 저장						
	pinfo->ClientID = info.ClientID;
	pinfo->X = info.X;
	pinfo->Y = info.Y;
	pinfo->Z = info.Z;
	// 캐릭터의 회전값을 저장
	pinfo->Yaw = info.Yaw;
	pinfo->Pitch = info.Pitch;
	pinfo->Roll = info.Roll;
	// 캐릭터의 속도를 저장
	pinfo->VX = info.VX;
	pinfo->VY = info.VY;
	pinfo->VZ = info.VZ;
	pinfo->HP = info.HP;
	pinfo->SkillType = info.SkillType;
	pinfo->IsSkilling = info.IsSkilling;
	pinfo->IsAttacking = info.IsAttacking;
	pinfo->clientPlayerType = info.clientPlayerType;
	CurrentStageLevel = info.StageLevel;

	WriteCharactersInfoToSocket(Socketinfo);
	Send(Socketinfo);
}

void Server::LogoutCharacter(stringstream& RecvStream, SOCKETINFO* Socketinfo)
{
	int SessionId;
	RecvStream >> SessionId;
	playerinfo.players[SessionId].IsAliving = false;
	SessionSocket.erase(SessionId);
//	printf("플레이어 수 : %d\n", SessionSocket.size());
	WriteCharactersInfoToSocket(Socketinfo);
}

void Server::BroadcastNewPlayer(cPlayer & player)
{
	stringstream SendStream;
	SendStream << PacketType::ENTER_NEW_PLAYER << endl;
	SendStream << player << endl;

	Broadcast(SendStream);
}

void Server::Broadcast(stringstream &SendStream)
{
	SOCKETINFO* client = new SOCKETINFO;
	for (const auto& kvp : SessionSocket)
	{
		client->socket = kvp.second;
		CopyMemory(client->messageBuffer, (CHAR*)SendStream.str().c_str(), SendStream.str().length());
		client->dataBuf.buf = client->messageBuffer;
		client->dataBuf.len = SendStream.str().length();
		Send(client);
	}
}
void Server::WriteCharactersInfoToSocket(SOCKETINFO * Socketinfo)
{
	stringstream SendStream;
	SendStream << PacketType::RECV_CHARACTER << endl;
	SendStream << playerinfo << endl;

	CopyMemory
	(
		Socketinfo->messageBuffer,
		(CHAR*)SendStream.str().c_str(),
		SendStream.str().length()
	);

	Socketinfo->dataBuf.buf = Socketinfo->messageBuffer;
	Socketinfo->dataBuf.len = SendStream.str().length();
}

void Server::DamageCharacter(stringstream& RecvStream, SOCKETINFO * Socketinfo)
{
	int CharacterID;
	RecvStream >> CharacterID;

	WriteCharactersInfoToSocket(Socketinfo);
	Send(Socketinfo);
}

void Server::NextStageCreateMonsterWorkerThread()
{
	unsigned int threadId;

	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);

	nThreadCnt = sysInfo.dwNumberOfProcessors *2;
	NextStageMonsterHandle = new HANDLE[nThreadCnt];
	for (int i = 0; i < 1; i++)
	{
		NextStageMonsterHandle[i] = (HANDLE *)_beginthreadex
		(
			NULL,
			0,
			&CallNextStageMonsterWorkerThread,
			this,
			CREATE_SUSPENDED,
			&threadId
		);

		if (NextStageMonsterHandle[i] == NULL)
		{
			return;
		}
		ResumeThread(NextStageMonsterHandle[i]);
	}
//	cout << "Next Stage Monster Thread Start" << endl;
}

bool Server::CreateWorkerThread()
{
	unsigned int threadId;
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);

	nThreadCnt = sysInfo.dwNumberOfProcessors *2 ;

	WorkerHandle = new HANDLE[nThreadCnt];
	for (int i = 0; i < nThreadCnt; i++)
	{
		WorkerHandle[i] = (HANDLE *)_beginthreadex
		(
			NULL,
			0,
			&CallWorkerThread,
			this,
			CREATE_SUSPENDED,
			&threadId
		);

		if (WorkerHandle[i] == NULL)
		{
			printf("[ERROR] Worker Thread 생성 실패\n");
			return false;
		}
		ResumeThread(WorkerHandle[i]);
	}
//	cout << "Worker Thread Start" << endl;
	return true;
}
void Server::CreateMonsterWorkerThread()
{
	unsigned int threadId;
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);

	nThreadCnt = sysInfo.dwNumberOfProcessors *2;

	MonsterHandle = new HANDLE[nThreadCnt];
	for (int i = 0; i < 1; i++)
	{
		MonsterHandle[i] = (HANDLE *)_beginthreadex
		(
			NULL,
			0,
			&CallMonsterWorkerThread,
			this,
			CREATE_SUSPENDED,
			&threadId
		);

		if (MonsterHandle[i] == NULL)
		{
			return;
		}
		ResumeThread(MonsterHandle[i]);
	}
}

//0 - GOLEM
//1 - VAMP
//2 - LIZARD

void Server::InitMonster()
{
	Monster InitMonsterInformation;

	//처음 게임 시작 시 방 몬스터들
	{
		InitMonsterInformation.X = 2006;
		InitMonsterInformation.Y = -10281;
		InitMonsterInformation.Z = -2309;
		InitMonsterInformation.MonsterID = 0;
		InitMonsterInformation.MonsterType = 1;
		Monsterinfo.monsters[InitMonsterInformation.MonsterID] = InitMonsterInformation;

		InitMonsterInformation.X = 1893;
		InitMonsterInformation.Y = -10420;
		InitMonsterInformation.Z = -2309;
		InitMonsterInformation.MonsterID = 1;
		InitMonsterInformation.MonsterType = 1;
		Monsterinfo.monsters[InitMonsterInformation.MonsterID] = InitMonsterInformation;

		InitMonsterInformation.X = 1982;
		InitMonsterInformation.Y = -10666;
		InitMonsterInformation.Z = -2309;
		InitMonsterInformation.MonsterID = 2;
		InitMonsterInformation.MonsterType = 1;
		Monsterinfo.monsters[InitMonsterInformation.MonsterID] = InitMonsterInformation;

	}
	
	//계단 위 철창
	{
		InitMonsterInformation.X = -2687;
		InitMonsterInformation.Y = -8350;
		InitMonsterInformation.Z = -1609;
		InitMonsterInformation.MonsterID = 3;
		InitMonsterInformation.MonsterType = 1;
		Monsterinfo.monsters[InitMonsterInformation.MonsterID] = InitMonsterInformation;
	}

	//쭉 올라가다가 왼쪽 큰 방
	{
		InitMonsterInformation.X = 192;
		InitMonsterInformation.Y = 980;
		InitMonsterInformation.Z = 390;
		InitMonsterInformation.MonsterID = 4;
		InitMonsterInformation.MonsterType = 2;
		Monsterinfo.monsters[InitMonsterInformation.MonsterID] = InitMonsterInformation;

		InitMonsterInformation.X = 19;
		InitMonsterInformation.Y = 1070;
		InitMonsterInformation.Z = 390;
		InitMonsterInformation.MonsterID = 5;
		InitMonsterInformation.MonsterType = 2;
		Monsterinfo.monsters[InitMonsterInformation.MonsterID] = InitMonsterInformation;

		InitMonsterInformation.X = -152;
		InitMonsterInformation.Y = 978;
		InitMonsterInformation.Z = 390;
		InitMonsterInformation.MonsterID = 6;
		InitMonsterInformation.MonsterType = 2;
		Monsterinfo.monsters[InitMonsterInformation.MonsterID] = InitMonsterInformation;
	}
	
	//왼쪽 방에서 쭉 더 들어와서 다음 방 몬스터들
	{
		InitMonsterInformation.X = 1697;
		InitMonsterInformation.Y = 4535;
		InitMonsterInformation.Z = 390;
		InitMonsterInformation.MonsterID = 7;
		InitMonsterInformation.MonsterType = 2;
		Monsterinfo.monsters[InitMonsterInformation.MonsterID] = InitMonsterInformation;
	}
}

void Server::NextStageInitMonster()
{
	NextStageMonster InitMonsterInformation;
	{
		//2스테이지 왼쪽 방
		{
			InitMonsterInformation.X = -9463;
			InitMonsterInformation.Y = -9265;
			InitMonsterInformation.Z = 1090;
			InitMonsterInformation.MonsterID = 8;
			NextStageMonsterinfo.monsters[InitMonsterInformation.MonsterID] = InitMonsterInformation;

			InitMonsterInformation.X = -9476;
			InitMonsterInformation.Y = -9734;
			InitMonsterInformation.Z = 1090;
			InitMonsterInformation.MonsterID = 9;
			NextStageMonsterinfo.monsters[InitMonsterInformation.MonsterID] = InitMonsterInformation;

		}
		//2스테이지 엔딩 전 큰 방 몬스터들
		{
			InitMonsterInformation.X = -6894;
			InitMonsterInformation.Y = -16025;
			InitMonsterInformation.Z = 1190;
			InitMonsterInformation.MonsterID = 10;
			NextStageMonsterinfo.monsters[InitMonsterInformation.MonsterID] = InitMonsterInformation;

			InitMonsterInformation.X = -6307;
			InitMonsterInformation.Y = -15943;
			InitMonsterInformation.Z = 1190;
			InitMonsterInformation.MonsterID = 11;
			NextStageMonsterinfo.monsters[InitMonsterInformation.MonsterID] = InitMonsterInformation;

			InitMonsterInformation.X = -7511;
			InitMonsterInformation.Y = -15886;
			InitMonsterInformation.Z = 1190;
			InitMonsterInformation.MonsterID = 12;
			NextStageMonsterinfo.monsters[InitMonsterInformation.MonsterID] = InitMonsterInformation;

		}
	}
}

void Server::MonsterThread()
{
	InitMonster();
	while (true)
	{
		for (auto & kvp : Monsterinfo.monsters)
		{
			auto & monster = kvp.second;

			for (auto & player : playerinfo.players)
			{
				if (monster.AttackingPlayerRange(player.second) && !monster.IsAttacking)
				{
					monster.MonsterAttackingPlayer(player.second);
					continue;
				}

				if (monster.ChasingPlayerRange(player.second) && !monster.IsAttacking)
				{
					monster.MoveTo(player.second);
					continue;
				}
			}			
		}
		stringstream SendStream;
		SendStream << PacketType::SYNCRO_MONSTER << endl;
		SendStream << Monsterinfo << endl;

		Broadcast(SendStream);
		Sleep(33);
	}
}
void Server::NextStageMonsterThread()
{
	NextStageInitMonster();

	while (true)
	{
		for (auto & kvp : NextStageMonsterinfo.monsters)
		{
			auto & monster = kvp.second;
			for (auto & player : playerinfo.players)
			{
				if (monster.AttackingPlayerRange(player.second) && !monster.IsAttacking)
				{
					monster.MonsterAttackingPlayer(player.second);
					continue;
				}

				if (monster.ChasingPlayerRange(player.second) && !monster.IsAttacking)
				{
					monster.MoveTo(player.second);
					continue;
				}
			}
		}
		stringstream SendStream;
		SendStream << PacketType::NEXT_LEVEL_STAGE_SYNCRO_MONSTER << endl;
		SendStream << NextStageMonsterinfo << endl;

		Broadcast(SendStream);

		Sleep(33);
	}
}