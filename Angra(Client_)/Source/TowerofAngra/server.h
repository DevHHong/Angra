// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
// *******************************************************
#define WIN32_LEAN_AND_MEAN
// *******************************************************
#include "TowerofAngra.h"
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <iostream>
#include <map>
#include "Runtime/Core/Public/HAL/Runnable.h"
#include "Runtime/Core/Public/HAL/ThreadSafeCounter.h"

class ACharacterPlayerController;

using namespace std;

//Network Define
#define SOCKET_BUF_SIZE			4096
#define SERVERPROT				9000
#define SERVERIP				"127.0.0.1"

// 소켓 통신 구조체
struct SOCKETIONFO
{
	WSAOVERLAPPED	overlapped;
	WSABUF			dataBuf;
	SOCKET			socket;
	char			messageBuffer[SOCKET_BUF_SIZE];
	int				recvBytes;
	int				sendBytes;
};

// 주고받을 패킷 타입.
enum PacketType
{
	ENROLL_CHARACTER,
	SEND_CHARACTER,
	RECV_CHARACTER,
	ENTER_NEW_PLAYER,
	LOGOUT_CHARACTER,
	SPAWN_MONSTER,
	HIT_PLAYER,
	HIT_MONSTER,
	HIT_NEXT_STAGE_MONSTER,
	SYNCRO_MONSTER,
	DESTROY_MONSTER,
	NEXT_LEVEL_STAGE_SYNCRO_MONSTER,
	NEXT_LEVEL_STAGE_SPAWN_MONSTER,
	NEXT_LEVEL_STAGE_DESTROY_MONSTER
};

class cPlayer
{
public:
	cPlayer() {};
	~cPlayer() {};

private:


public:
	//아이디
	int ClientID = 0;

	// PlayerType
	int clientPlayerType = 0;
	FVector Location;		// 위치
	FRotator Rotation;		// 회전
	FVector Velocity;		// 속도

	float HP;
	bool IsSkilling;
	bool IsAttacking;
	bool IsAliving;
	// SkillTYPE
	int SkillType = 0; // 0 - None 1 - CommonSkill 2 - Skill

	int StageLevel;

	//패킷 직렬화 역직렬화
	friend ostream& operator<<(ostream &stream, cPlayer& info)
	{
		stream << info.ClientID << endl;

		stream << info.Location.X << endl;
		stream << info.Location.Y << endl;
		stream << info.Location.Z << endl;

		stream << info.Velocity.X << endl;
		stream << info.Velocity.Y << endl;
		stream << info.Velocity.Z << endl;

		stream << info.Rotation.Yaw << endl;
		stream << info.Rotation.Pitch << endl;
		stream << info.Rotation.Roll << endl;

		stream << info.HP << endl;

		stream << info.SkillType << endl;
		stream << info.StageLevel << endl;
		stream << info.IsSkilling << endl;
		stream << info.IsAttacking << endl;
		stream << info.clientPlayerType << endl;
		return stream;
	}

	friend istream& operator>>(istream& stream, cPlayer& info)
	{
		stream >> info.ClientID;

		stream >> info.Location.X;
		stream >> info.Location.Y;
		stream >> info.Location.Z;

		stream >> info.Velocity.X;
		stream >> info.Velocity.Y;
		stream >> info.Velocity.Z;

		stream >> info.Rotation.Yaw;
		stream >> info.Rotation.Pitch;
		stream >> info.Rotation.Roll;

		stream >> info.HP;

		stream >> info.SkillType;
		stream >> info.StageLevel;
		stream >> info.IsSkilling;
		stream >> info.IsAttacking;
		stream >> info.clientPlayerType;
		return stream;
	}
};


class cPlayerInfo
{
public:
	cPlayerInfo() {};
	~cPlayerInfo() {};

	map<int, cPlayer> players;

	friend ostream& operator<<(ostream &stream, cPlayerInfo& info)
	{
		stream << info.players.size() << endl;
		for (auto& kvp : info.players)
		{
			stream << kvp.first << endl;
			stream << kvp.second << endl;
		}

		return stream;
	}

	friend istream &operator>>(istream &stream, cPlayerInfo& info)
	{
		int nPlayers = 0;
		int SessionId = 0;
		cPlayer Player;
		info.players.clear();

		stream >> nPlayers;
		for (int i = 0; i < nPlayers; i++)
		{
			stream >> SessionId;
			stream >> Player;
			info.players[SessionId] = Player;
		}

		return stream;
	}
};

class Monster
{
public:
	float X;
	float Y;
	float Z;

	int MonsterID;
	bool IsAttacking;
	float HP;

	// MonsterTYPE
	int MonsterType = 0;


	friend ostream& operator<<(ostream &stream, Monster& info)
	{
		stream << info.X << endl;
		stream << info.Y << endl;
		stream << info.Z << endl;
		stream << info.MonsterID << endl;
		stream << info.IsAttacking << endl;
		stream << info.MonsterType << endl;

		return stream;
	}

	friend istream& operator>>(istream& stream, Monster& info)
	{
		stream >> info.X;
		stream >> info.Y;
		stream >> info.Z;
		stream >> info.MonsterID;
		stream >> info.IsAttacking;
		stream >> info.MonsterType;

		return stream;
	}
};

class MonsterSet
{
public:
	map<int, Monster> monsters;

	friend ostream& operator<<(ostream &stream, MonsterSet& info)
	{
		stream << info.monsters.size() << endl;
		for (auto& kvp : info.monsters)
		{
			stream << kvp.first << endl;
			stream << kvp.second << endl;
		}

		return stream;
	}

	friend istream& operator>>(istream& stream, MonsterSet& info)
	{
		int nMonsters = 0;
		int PrimaryId = 0;
		Monster monster;
		info.monsters.clear();

		stream >> nMonsters;
		for (int i = 0; i < nMonsters; i++)
		{
			stream >> PrimaryId;
			stream >> monster;
			info.monsters[PrimaryId] = monster;
		}

		return stream;
	}
};
//=======================================================================
class NextStageMonster
{
public:
	float X;
	float Y;
	float Z;
	float Movement;

	int MonsterID;
	bool IsAttacking;
	float HP;
	float AttackRange;
	float ChaseRange;
	bool Chasing;
	float AttackPower;


	// MonsterTYPE
	int MonsterType = 0;



	friend ostream& operator<<(ostream &stream, NextStageMonster& info)
	{
		stream << info.X << endl;
		stream << info.Y << endl;
		stream << info.Z << endl;
		stream << info.MonsterID << endl;
		stream << info.IsAttacking << endl;
		//		stream << info.HP << endl;
		return stream;
	}

	friend istream& operator>>(istream& stream, NextStageMonster& info)
	{
		stream >> info.X;
		stream >> info.Y;
		stream >> info.Z;
		stream >> info.MonsterID;
		stream >> info.IsAttacking;
		//		stream >> info.HP;
		return stream;
	}
};
class NextStageMonsterSet
{
public:
	map<int, NextStageMonster> monsters;

	friend ostream& operator<<(ostream &stream, NextStageMonsterSet& info)
	{
		stream << info.monsters.size() << endl;
		for (auto& kvp : info.monsters)
		{
			stream << kvp.first << endl;
			stream << kvp.second << endl;
		}

		return stream;
	}

	friend istream& operator>>(istream& stream, NextStageMonsterSet& info)
	{
		int nMonsters = 0;
		int PrimaryId = 0;
		NextStageMonster monster;
		info.monsters.clear();

		stream >> nMonsters;
		for (int i = 0; i < nMonsters; i++)
		{
			stream >> PrimaryId;
			stream >> monster;
			info.monsters[PrimaryId] = monster;
		}

		return stream;
	}
};

class TOWEROFANGRA_API server : public FRunnable
{
private:
	SOCKET	Sock;						// 서버와 연결할 소켓	
	cPlayerInfo playerinfo;				// 케릭터 정보
	char 	recvBuffer[SOCKET_BUF_SIZE];		// 수신 버퍼 스트림
	cPlayer * RecvNewPlayer(stringstream &);
	cPlayer NewPlayer;
	ACharacterPlayerController* Controller;	// 플레이어 컨트롤러 정보
	MonsterSet	MonsterSetInfo;
	Monster		MonsterInfo;


	//========================================================================다음 스테이지
	NextStageMonsterSet		NextStageMonsterSetInfo;
	NextStageMonster		NextStageMonsterInfo;
	//========================================================================다음 스테이지

public:
	server();
	virtual ~server();

public:
	int nTotalSockets = 0;
	SOCKETIONFO* SocketInfoArray[WSA_MAXIMUM_WAIT_EVENTS];
	WSAEVENT EventArray[WSA_MAXIMUM_WAIT_EVENTS];


	//소켓 초기화
	bool InitSock();
	bool Connect(const char*, int);

	void EnrollCharacterInfo(cPlayer&);
	void LogoutCharacter(int);
	void SendPlayer(cPlayer& info);
	MonsterSet* RecvMonsterSet(stringstream&);
	Monster* RecvMonster(stringstream&);

	//========================================================================다음 스테이지
	NextStageMonsterSet* NextStageRecvMonsterSet(stringstream&);
	NextStageMonster* NextStageRecvMonster(stringstream&);
	//========================================================================다음 스테이지

	// 스레드 시작 및 종료
	bool StartListen();
	void StopListen();

	void CloseSocket();
	cPlayerInfo* RecvCharacterInfo(stringstream&);

	// FRunnable Thread members	
	FRunnableThread* Thread;
	FThreadSafeCounter StopTaskCounter;

	//FRunnable override 함수
	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	virtual void Exit();
	//======================

	// 플레이어 컨트롤러 세팅
	void SetPlayerController(ACharacterPlayerController*);

	//-----------------------------------------------08-03
	void HitMonster(const int & MonsterID, const float& Hp, const float& damage, const bool& isDie);
	void HitPlayer(const int&);
	//-----------------------------------------------
	void NextStageHitMonster(const int & MonsterID, const float& Hp, const float& damage, const bool& isDie);
	static server* GetSingleton()
	{
		static server ins;
		return &ins;
	}

};