// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Character.h"
#include "HMonster.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);

UENUM(BlueprintType)
enum class EMonsterName : uint8
{
	GOLEM,
	VAMP,
	LIZARD,
	DEMON,
	NONE
};

UCLASS()
class TOWEROFANGRA_API AHMonster : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AHMonster();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	void DieOn();
public:	
	int MonsterID;
	float HP;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PostInitializeComponents() override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void PossessedBy(AController* NewController) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	void Attack();
	void ServerAttack(EMonsterName MonsterType);
	FOnAttackEndDelegate OnAttackEnd;

	void DamageAnim();
protected:
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void AttackCheck();

	void SkillCheck();
protected:
	UPROPERTY(VisibleAnywhere, Category = State)
	class UHCharaterStateComponent* CharacterState;

	UPROPERTY(VisibleAnywhere, Category = UI)
	class UWidgetComponent* HPBarWidget;

	UPROPERTY(VisibleAnywhere, Category = Anim)
	class UAnimInstance* AnimInstance{ nullptr };

	UPROPERTY()
	class AHMonsterAIController* MonsterAIController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State, Meta = (AllowPrivateAccess = true))
	float DeadTimer;

	FTimerHandle DeadTimerHandle = {};

	UPROPERTY(VisibleAnywhere, Category = Effect)
	class UParticleSystemComponent* AttackDamageEffect;

	UPROPERTY(VisibleAnywhere, Category = Effect)
	class UParticleSystemComponent* DamageEffect;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsHP_Show;

	UPROPERTY()
	float DetectRadius;		// AI로 찾는 범위

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float AttackRadius;		// 공격 반지름

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float fAttackPower;		// 공격력

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Damage, Meta = (AllowPrivateAccess = true))
	float FinalDamage;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Body, Meta = (AllowPrivateAccess = true))
	float BodyCenter;

public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = HP, Meta = (AllowPrivateAccess = true))
	float CurrentHP;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsAttacking;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = State, Meta = (AllowPrivateAccess = true))
	bool IsAliving;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = State, Meta = (AllowPrivateAccess = true))
	bool IsDie;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = State, Meta = (AllowPrivateAccess = true))
	EMonsterName CurrentMonsterType;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsServerAttacking;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float AttackRange;		// 공격 범위

public:
	void SetHpShow(bool HpShow) { IsHP_Show = HpShow; }

	float GetDetectRadius()const { return DetectRadius; }

	float GetAttackRange()const { return AttackRange; }

	void MoveToLocation(const FVector& dest);

	void ServerSendDieOn(EMonsterName MonsterType);

	void MonsterDamageEffect(const float& damage);

	void ServerHpShowTick();
};
