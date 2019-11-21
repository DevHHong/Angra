#include"pch.h"
#include "Monster.h"



Monster::Monster()
	:X(10),
	Y(0)
	,Z(0)
	,Movement(15),
	HP(0),
	AttackRange(70),
	IsAttacking(false),
	ChaseRange(600),
	AttackPower(10),
	IsAliving(true),
	IsDie(false)
{
}


Monster::~Monster()
{
}


bool Monster::IsAlive()
{
	if (HP <= 0)
	{
		return false;
	}
	return true;
}

bool Monster::IsAttack()
{
	return IsAttacking;
}

void Monster::MoveTo(const cPlayer& Target)
{
	if (Target.X > X)
		X += Movement;
	if (Target.X < X)
		X -= Movement;
	if (Target.Y > Y)
		Y += Movement;
	if (Target.Y < Y)
		Y -= Movement;
	if (Target.Z > Z)
		Z += Movement;
	if (Target.Z < Z)
		Z -= Movement;

}
void Monster::MonsterAttackingPlayer(cPlayer& Target)
{
	std::thread t([&]() 
	{	
		IsAttacking = true;
		//Target.HP -= AttackPower;		
//		cout << MonsterID<<" 번째 Monster HIT " << endl;
		std::this_thread::sleep_for(1s);
		IsAttacking = false;
	});
	
	t.detach();
}

void Monster::RecvDamage(float damage)
{
	HP -= damage;
//	cout << "Monster Receive Damage ! Remain HP : " << HP << endl;
}

void Monster::RecvMonsterIsDie(bool isDie)
{
	IsDie = isDie;
}

bool Monster::AttackingPlayerRange(const cPlayer&Target)
{
	float result = sqrt((X - Target.X)*(X - Target.X) + (Y - Target.Y)*(Y - Target.Y));

	if (result < AttackRange && !IsDie)
	{
		return true;
	}
		
	return false;
}
bool  Monster::ChasingPlayerRange(const cPlayer&Target)
{

	float result = sqrt((X - Target.X)*(X - Target.X) + (Y - Target.Y)*(Y - Target.Y));

	if (result < ChaseRange && !IsDie)
		return true;
	return false;

}

//==============================================================NEXT STAGE MONSTER


NextStageMonster::NextStageMonster()
	:X(10),
	Y(0)
	, Z(0)
	, Movement(15),
	HP(0),
	AttackRange(70),
	IsAttacking(false),
	Chasing(false),
	ChaseRange(1100),
	AttackPower(10),
	IsAliving(true),
	IsDie(false)
{
}


NextStageMonster::~NextStageMonster()
{
}


bool NextStageMonster::IsAlive()
{
	if (HP <= 0)
	{
		return false;
	}
	return true;
}

bool NextStageMonster::IsAttack()
{
	return IsAttacking;
}

void NextStageMonster::MoveTo(const cPlayer& Target)
{
	if (Target.X > X)
		X += Movement;
	if (Target.X < X)
		X -= Movement;
	if (Target.Y > Y)
		Y += Movement;
	if (Target.Y < Y)
		Y -= Movement;
	if (Target.Z > Z)
		Z += Movement;
	if (Target.Z < Z)
		Z -= Movement;

//		cout << " monster.X :  " << X << endl;
}
void NextStageMonster::MonsterAttackingPlayer(cPlayer& Target)
{
	std::thread t([&]()
	{
		IsAttacking = true;
		//Target.HP -= AttackPower;
		//cout << "Next공격 당함" << endl;
		std::this_thread::sleep_for(1s);
		IsAttacking = false;
	});

	t.detach();
}

void NextStageMonster::RecvDamage(float damage)
{
	HP -= damage;
//	cout << "Monster Receive Damage ! Remain HP : " << HP << endl;
}

void NextStageMonster::RecvMonsterIsDie(bool isDie)
{
	IsDie = isDie;
}

bool NextStageMonster::AttackingPlayerRange(const cPlayer&Target)
{
	/*if (abs(Target.X - X) < AttackRange && abs(Target.Y - Y) < AttackRange)
		return true;*/

	float result = sqrt((X - Target.X)*(X - Target.X) + (Y - Target.Y)*(Y - Target.Y));

	if (result < AttackRange && !IsDie)
		return true;

	return false;
}
bool  NextStageMonster::ChasingPlayerRange(const cPlayer&Target)
{
	
	float result = sqrt((X - Target.X)*(X - Target.X) + (Y - Target.Y)*(Y - Target.Y));

	if (result < ChaseRange && !IsDie)
		return true;
	return false;
}