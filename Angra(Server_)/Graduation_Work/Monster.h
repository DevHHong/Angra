#pragma once
#include"pch.h"
#include <WinSock2.h>
#include<map>


using namespace std;

class Monster
{
public:
	Monster();
	~Monster();
public:
	float X;
	float Y;
	float Z;
	float Movement;

	int MonsterID;
	bool IsAttacking;
	bool IsAliving;
	float HP;
	float AttackRange;
	float ChaseRange;

	float AttackPower;
	bool IsDie{ false };



public:
	bool AttackingPlayerRange(const cPlayer&);
	bool ChasingPlayerRange(const cPlayer&);
	void MonsterAttackingPlayer(cPlayer&);
	void RecvDamage(float damage);
	void RecvMonsterIsDie(bool isDie);
public:
	//몬스터가 살아 있는지
	bool IsAlive();
	//몬스터가 공격중인지
	bool IsAttack();
	// 플레이어한테 이동
	void MoveTo(const cPlayer& Target);

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
	NextStageMonster();
	~NextStageMonster();
public:
	float X;
	float Y;
	float Z;
	float Movement;

	int MonsterID;
	bool IsAttacking;
	bool IsAliving;
	float HP;
	float AttackRange;
	float ChaseRange;
	bool Chasing;
	float AttackPower;
	bool IsDie{ false };
public:
	bool AttackingPlayerRange(const cPlayer&);
	bool ChasingPlayerRange(const cPlayer&);
	void MonsterAttackingPlayer(cPlayer&);
	void RecvDamage(float damage);
	void RecvMonsterIsDie(bool isDie);
public:
	//몬스터가 살아 있는지
	bool IsAlive();
	//몬스터가 공격중인지
	bool IsAttack();
	// 플레이어한테 이동
	void MoveTo(const cPlayer& Target);

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
//		stream << info.MonsterType << endl;
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
//		stream >> info.MonsterType;
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