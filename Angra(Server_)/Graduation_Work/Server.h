#pragma once
#include"pch.h"
#include <WinSock2.h>
#include<map>

// ��Ƽ����Ʈ ���� ���� define
#define _WINSOCK_DEPRECATED_NO_WARNINGS
// DB����
// DBconnector Ŭ������ ����
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

// IOCP ���� ����ü
struct SOCKETINFO
{
	WSAOVERLAPPED	overlapped;
	WSABUF			dataBuf;
	SOCKET			socket;
	char			messageBuffer[SOCKET_BUF_SIZE];
	int				recvBytes;
	int				sendBytes;
};

// �ְ���� ��Ŷ Ÿ��.
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
	//���̵�
	int ClientID = 0;

	// PlayerType
	int clientPlayerType = 0;

	// ��ġ
	float X;
	float Y;
	float Z;
	// ȸ����
	float Yaw;
	float Pitch;
	float Roll;

	// �ӵ�
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
	//��Ŷ ����ȭ ������ȭ
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

	// ���� ��� �� ���� ���� ����
	bool Initialize();
	// ���� ����
	void StartServer();
	// �۾� ������ ����
	bool CreateWorkerThread();
	// �۾� ������
	void WorkerThreadFunc();
	//���ͽ�����
	void CreateMonsterWorkerThread();
	
	//���� �������� ���� ������
	void NextStageCreateMonsterWorkerThread();
	
	// Ŭ���̾�Ʈ���� �۽�
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

	SOCKETINFO *	SocketInfo;		// ���� ����
	SOCKET			ListenSocket;		// ���� ���� ����
	HANDLE			Iocp;			// IOCP ��ü �ڵ�
	bool			Accept;			// ��û ���� �÷���
	bool			WorkerThread;	// �۾� ������ ���� �÷���
	HANDLE *		WorkerHandle;	// �۾� ������ �ڵ�	
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
