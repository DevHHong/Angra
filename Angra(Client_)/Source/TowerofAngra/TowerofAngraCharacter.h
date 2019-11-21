// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "TowerofAngra.h"
#include "GameFramework/Character.h"
#include "TowerofAngraCharacter.generated.h"

UENUM(BlueprintType)
enum class ESkillType : uint8
{
	NONE,
	COMMONSKILL,
	SKILL,
};

UCLASS(config=Game)
class ATowerofAngraCharacter : public ACharacter
{
	GENERATED_BODY()
	
public:
	ATowerofAngraCharacter();
	
/* ================ HoHyuk ================ */
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
/* ---------------------- 카메라 ---------------------- */
public:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* Camera;
protected:
	// 카메라 모드 
	enum class ECAMMode
	{
		DYNAMIC_CAM,
		STATIC_CAM,
	};

	void SetCamMode(ECAMMode NewCamMode);
	ECAMMode CurrentCamMode = ECAMMode::STATIC_CAM;
	FVector DirectionToMove = FVector::ZeroVector;

	float ArmLengthTo = 0.f;
	FRotator ArmRotationTo = FRotator::ZeroRotator;
	float ArmLengthSpeed = 0.f;
	float ArmRotationSpeed = 0.f;
/* ---------------------- 카메라 ---------------------- */

/* ---------------------- 입력 ---------------------- */
protected:
	void MoveForward(float NewAxisValue);
	void MoveRight(float NewAxisValue);
	void LookUp(float NewAxisValue);
	void Turn(float NewAxisValue);
	void ViewChange();
	void Cheat();
public:
	void Jump();
/* ---------------------- 입력 ---------------------- */

/* ---------------------- 캐릭터 공통 변수 ---------------------- */
public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsAttacking;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsServerSend_Attacking;	// 서버로 보내지는 공격판별

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Skill, Meta = (AllowPrivateAccess = true))
	bool IsSkilling;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Damage, Meta = (AllowPrivateAccess = true))
	float FinalDamage;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Mana, Meta = (AllowPrivateAccess = true))
	float FinalMana;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float fAttackPower;		// 공격력

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Skill, Meta = (AllowPrivateAccess = true))
	float fSkillPower;		// 스킬 공격력

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Skill, Meta = (AllowPrivateAccess = true))
	float fCommonSkillPower;		// 공통스킬 공격력

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Skill, Meta = (AllowPrivateAccess = true))
	float CommonSkillMP;		// 공통스킬 마나소모

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Skill, Meta = (AllowPrivateAccess = true))
	float SkillMP;		// 스킬 마나소모

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Skill, Meta = (AllowPrivateAccess = true))
	ESkillType SKILL_TYPE;		// 서버로 보내주는 스킬 타입

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool CanNextCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsComboInputOn;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 CurrentCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 MaxCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = HP, Meta = (AllowPrivateAccess = true))
	float CurrentHP;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = HP, Meta = (AllowPrivateAccess = true))
	bool IsFillHP;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = HP, Meta = (AllowPrivateAccess = true))
	bool IsFillMP;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = HP, Meta = (AllowPrivateAccess = true))
	float FillHP_Limit;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = HP, Meta = (AllowPrivateAccess = true))
	float FillMP_Limit;

	UPROPERTY(VisibleAnywhere, Category = Effect)
	class UParticleSystemComponent* AttackDamageEffect;
/* ---------------------- 캐릭터 공통 변수 ---------------------- */

/* ---------------------- 캐릭터 데미지 함수 ---------------------- */
public:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;
/* ---------------------- 캐릭터 데미지 함수 ---------------------- */

/* ---------------------- 무기 변수 ---------------------- */
public:
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	UStaticMeshComponent* Weapon;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	UStaticMeshComponent* SubWeapon;

/* ---------------------- 무기 변수 ---------------------- */

/* ---------------------- Tick 함수 ---------------------- */
private:
	void SpringArmTick(float DeltaTime);
	void StateTick(float DeltaTime);

/* ---------------------- Tick 함수 ---------------------- */

/* ---------------------- virtual 함수 ---------------------- */
public:
	virtual void InitCommon();
	virtual void Attack();
	virtual void OtherPlayerAttack(int AttackCount);
	virtual void Skill();
	virtual void CommonSkill();
	virtual void CommonSkillCheck();		// 스킬 충돌체크
	virtual void ServerRecvSkillCheck(ESkillType skill_type);
	virtual void ServerSetHP(float hp);
	virtual void WeaponChange();
/* ---------------------- virtual 함수 ---------------------- */

/* ---------------------- 현재 나의 캐릭터 함수 ---------------------- */
public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = PlayerType, Meta = (AllowPrivateAccess = true))
	EPlayerType CurrentPlayerType{ EPlayerType::NONE };

/* ---------------------- 현재 나의 캐릭터 함수 ---------------------- */

/* ---------------------- UCameraShake ---------------------- */
public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UCameraShake> CameraShake;
/* ---------------------- UCameraShake ---------------------- */
/* ================ HoHyuk ================ */

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	class ASkillEffect* Effect;

	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
protected:
	// UI관리
	UPROPERTY(VisibleAnywhere, Category = State)
	class UHCharaterStateComponent* CharacterState;

	UPROPERTY(VisibleAnywhere, Category = UI)
	class UWidgetComponent* UI_Widget;

	UPROPERTY(VisibleAnywhere, Category = UI)
	class UWidgetComponent* OtherPlayerHPBarWidget;

	// Sound
public:
	class USoundBase* HowlSound;
	class USoundBase* AtkSound;
	class USoundBase* JumpSound;
	class USoundBase* EatSound;
/* ======= Server ======= */
public:
	int SessionId;
/* ======= Server ======= */
};

