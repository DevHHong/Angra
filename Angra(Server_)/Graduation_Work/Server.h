#pragma once
#include"pch.h"
#include <WinSock2.h>
#include<map>

// 멀티바이트 집합 사용시 define
#define _WINSOCK_DEPRECATED_NO_WARNINGS
// DB정보
// DBconnector 클래스랑 연결
#define DB_ADDRESS		"localhost"
#define DB_PORT			 8809
#define DB_ID			"root"
#define DB_PASSWORD		"rhrnak"
#define DB_TITLE		"graduation"

using namespace std;
class MonsterSet;
class NextStageMonsterSet;
class Monster;

#include <mutex>

// IOCP 소켓 구조체
struct SOCKETINFO
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

	// 위치
	float X;
	float Y;
	float Z;
	// 회전값
	float Yaw;
	float Pitch;
	float Roll;

	// 속도
	float VX;
	float VY;
	float VZ;

	float HP;
	bool IsSkilling;
	bool IsAttacking;

	bool IsAliving;

	// SkillTYPE
	int SkillType = 0; // 0 - None 1 - CommonSkill 2 - Skill

	int StageLevel ;
	//패킷 직렬화 역직렬화
	friend ostream& operator<<(ostream &stream, cPlayer& info)
	{
		stream << info.ClientID << endl;

		stream << info.X << endl;
		stream << info.Y << endl;
		stream << info.Z << endl;

		stream << info.VX << endl;
		stream << info.VY << endl;
		stream << info.VZ << endl;

		stream << info.Yaw << endl;
		stream << info.Pitch << endl;
		stream << info.Roll << endl;

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

		stream >> info.X;
		stream >> info.Y;
		stream >> info.Z;

		stream >> info.VX;
		stream >> info.VY;
		stream >> info.VZ;

		stream >> info.Yaw;
		stream >> info.Pitch;
		stream >> info.Roll;

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

class Server : public singletonBase<Server>
{

public:
	Server();
	~Server();

	// 소켓 등록 및 서버 정보 설정
	bool Initialize();
	// 서버 시작
	void StartServer();
	// 작업 스레드 생성
	bool CreateWorkerThread();
	// 작업 스레드
	void WorkerThreadFunc();
	//몬스터스레드
	void CreateMonsterWorkerThread();
	
	//다음 스테이지 몬스터 스레드
	void NextStageCreateMonsterWorkerThread();
	
	// 클라이언트에게 송신
	static void Send(SOCKETINFO *);
	static void Recv(SOCKETINFO*);
	static void EnrollCharacter(stringstream &, SOCKETINFO *);
	static void LogoutCharacter(stringstream&, SOCKETINFO*);
	static void WriteCharactersInfoToSocket(SOCKETINFO *);
	static void BroadcastNewPlayer(cPlayer &);
	static void Broadcast(stringstream &);
	static void SyncCharacters(stringstream &, SOCKETINFO *);
	static void DamageCharacter(stringstream&, SOCKETINFO *);
	void MonsterThread();
	void NextStageMonsterThread();
protected:
	int nThreadCnt;

private:

	SOCKETINFO *	SocketInfo;		// 소켓 정보
	SOCKET			ListenSocket;		// 서버 리슨 소켓
	HANDLE			Iocp;			// IOCP 객체 핸들
	bool			Accept;			// 요청 동작 플래그
	bool			WorkerThread;	// 작업 스레드 동작 플래그
	HANDLE *		WorkerHandle;	// 작업 스레드 핸들	
	static cPlayerInfo		playerinfo;
	static map<int, SOCKET> SessionSocket;

private:
	static bool NextStageMonsterSpawn;
	HANDLE* MonsterHandle;
	HANDLE* NextStageMonsterHandle;

	static MonsterSet Monsterinfo;
	static NextStageMonsterSet NextStageMonsterinfo;

	void InitMonster();
	static void NextStageInitMonster();
	static void HitMonster(stringstream&, SOCKETINFO*);
	static void NextStageHitMonster(stringstream&, SOCKETINFO*);
	static void HitPlayer(stringstream&, SOCKETINFO*);

private:
	static int CurrentStageLevel;
};
