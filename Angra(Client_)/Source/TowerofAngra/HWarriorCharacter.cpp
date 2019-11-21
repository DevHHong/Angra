// Fill out your copyright notice in the Description page of Project Settings.

#include "HWarriorCharacter.h"
#include "HWarriorAnimInstance.h"
#include "HCharaterStateComponent.h"
#include "HMonster.h"

AHWarriorCharacter::AHWarriorCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_WARRIOR(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Tusk.SK_CharM_Tusk"));
	if (SK_WARRIOR.Succeeded())
		GetMesh()->SetSkeletalMesh(SK_WARRIOR.Object);

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> WARRIOR_ANIM(TEXT("/Game/TowerofAngra/Character/Warrior/Animations/Warrior_AnimBlueprint.Warrior_AnimBlueprint_C"));
	if (WARRIOR_ANIM.Succeeded())
		GetMesh()->SetAnimInstanceClass(WARRIOR_ANIM.Class);

	// Weapon
	FName WeaponSocket(TEXT("hand_rSocket"));
	if (GetMesh()->DoesSocketExist(WeaponSocket))
	{
		Weapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WEAPON"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_WEAPON(TEXT("/Game/TowerofAngra/Weapon/Swords/Blade_BlackKnight/SM_BlackKnight.SM_BlackKnight"));

		if (SM_WEAPON.Succeeded())
		{
			Weapon->SetStaticMesh(SM_WEAPON.Object);
		}

		Weapon->SetupAttachment(GetMesh(), WeaponSocket);
	}

	// SubWeapon
	SubWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SUBWEAPON"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_SUBWEAPON(TEXT("/Game/InfinityBladeWeapons/Weapons/Blade/Swords/Blade_BlackWyrmBlade/SM_BlackWyrmBlade.SM_BlackWyrmBlade"));
	if (SM_SUBWEAPON.Succeeded())
	{
		SubWeapon->SetStaticMesh(SM_SUBWEAPON.Object);
		SubWeapon->bHiddenInGame = true;		// 숨겨놓는다.
	}

	// Sound
	static ConstructorHelpers::FObjectFinder<USoundBase> JUMPSOUND(TEXT("/Game/HumanVocalizations/HumanMaleB/Wavs/voice_male_b_effort_jump_07.voice_male_b_effort_jump_07"));
	if (JUMPSOUND.Succeeded())
		JumpSound = JUMPSOUND.Object;

	InitCommon();
}

void AHWarriorCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AHWarriorCharacter::InitCommon()
{
	Super::InitCommon();

	MaxCombo = 4;			// 콤보 개수
	fAttackPower = 50.f;			// 기본 공격력
	fSkillPower = 110.f;			// 스킬 공격력
	SkillMP = 30.f;					// 마나 소모

	IsServerSend_Attacking = false;
	AttackEndComboState();
}

void AHWarriorCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHWarriorCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	WarriorAnim = Cast<UHWarriorAnimInstance>(GetMesh()->GetAnimInstance());
	if (WarriorAnim == nullptr) return;

	WarriorAnim->OnMontageEnded.AddDynamic(this, &AHWarriorCharacter::OnAttackMontageEnded);
	WarriorAnim->OnMontageEnded.AddDynamic(this, &AHWarriorCharacter::OnSkillMontageEnded);

	WarriorAnim->OnNextAttackCheck.AddLambda([this]() -> void {
		CanNextCombo = false;
		
		if (IsComboInputOn)
		{
			AttackStartComboState();
			WarriorAnim->JumpToAttackMontageSection(CurrentCombo);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("CurrentCombo : %d"), CurrentCombo));
			TOALOG(Warning,TEXT("Combo : %d"),CurrentCombo);
		}
	});

	WarriorAnim->OnAttackHitCheck.AddUObject(this, &AHWarriorCharacter::AttackCheck);
	WarriorAnim->OnCommonSkillCheck.AddUObject(this, &AHWarriorCharacter::CommonSkillCheck);
}

void AHWarriorCharacter::SetupPlayerInputComponent(UInputComponent * PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AHWarriorCharacter::Attack);
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Released, this, &AHWarriorCharacter::NotAttack);

	PlayerInputComponent->BindAction(TEXT("Skill"), EInputEvent::IE_Pressed, this, &AHWarriorCharacter::Skill);
	PlayerInputComponent->BindAction(TEXT("CommonSkill"), EInputEvent::IE_Pressed, this, &AHWarriorCharacter::CommonSkill);
}

void AHWarriorCharacter::Attack()
{
	Super::Attack();
	
	IsServerSend_Attacking = true;
	if (IsSkilling) return;

	if (IsAttacking)
	{
		if (CanNextCombo)
			IsComboInputOn = true;
	}
	else
	{
		AttackStartComboState();
		WarriorAnim->PlayAttackMontage();
		WarriorAnim->JumpToAttackMontageSection(CurrentCombo);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("CurrentCombo : %d"), CurrentCombo));
		IsAttacking = true;
	}
}

void AHWarriorCharacter::Skill()
{
	Super::Skill();

	if (IsSkilling) return;
	if (IsAttacking) return;
	if (CharacterState->GetMP() <= SkillMP)return;

	if (CameraShake != NULL)
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(CameraShake, 1.f);
	FinalMana += SkillMP;

	WarriorAnim->PlaySkillMontage();

	SkillCheck();

	IsSkilling = true;
}

void AHWarriorCharacter::CommonSkill()
{
	Super::CommonSkill();
	if (IsSkilling) return;
	if (IsAttacking) return;
	if (CharacterState->GetMP() <= CommonSkillMP)return;

	FinalMana += CommonSkillMP;
	WarriorAnim->PlayCommonSkillMontage();
	IsSkilling = true;
}

void AHWarriorCharacter::OtherPlayerAttack(int AttackCount)
{
	Super::OtherPlayerAttack(AttackCount);
	
	if (IsAttacking)
	{
		if (CanNextCombo)
			IsComboInputOn = true;
	}
	else
	{
		AttackStartComboState();
		WarriorAnim->PlayAttackMontage();
		WarriorAnim->JumpToAttackMontageSection(CurrentCombo);
		IsAttacking = true;
	}
}

void AHWarriorCharacter::NotAttack()
{
	IsServerSend_Attacking = false;
}

void AHWarriorCharacter::OnAttackMontageEnded(UAnimMontage * Montage, bool bInterrupted)
{
	// 공격이 끝나는 모션
	IsAttacking = false;
	IsServerSend_Attacking = false;
	
	AttackEndComboState();
}

void AHWarriorCharacter::AttackStartComboState()
{
	CanNextCombo = true;
	IsComboInputOn = false;

	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
}

void AHWarriorCharacter::AttackEndComboState()
{
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentCombo = 0;
}

void AHWarriorCharacter::AttackCheck()
{
	// 충돌체크
	float FinalAttackRange = 80.f;
	float AttackRadius = 50.f;
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * FinalAttackRange,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(AttackRadius),
		Params
	);

#if ENABLE_DRAW_DEBUG

	//FVector TraceVec = GetActorForwardVector() * FinalAttackRange;
	//FVector Center = GetActorLocation() + TraceVec * 0.5f;
	//float HalfHeight = FinalAttackRange * 0.5f + AttackRadius;
	//FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	//FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	//float DebugLifeTime = 3.f;

	//DrawDebugCapsule(GetWorld(),
	//	Center,
	//	HalfHeight,
	//	AttackRadius,
	//	CapsuleRot,
	//	DrawColor,
	//	false,
	//	DebugLifeTime);

#endif
	AHMonster* Monster = Cast<AHMonster>(HitResult.Actor);
	if (bResult && Monster)
	{
		if (HitResult.Actor.IsValid())
		{
			FDamageEvent DamageEvent;
			if (CurrentGameMode == EGameMode::MULTI_GAME)
				Monster->MonsterDamageEffect(fAttackPower);
			else
				HitResult.Actor->TakeDamage(fAttackPower, DamageEvent, GetController(), this);
		}
	}
}

void AHWarriorCharacter::OnSkillMontageEnded(UAnimMontage * Montage, bool bInterrupted)
{
	IsSkilling = false;
	SKILL_TYPE = ESkillType::NONE;
	OnSkillEnd.Broadcast();
}

void AHWarriorCharacter::SkillCheck()
{
	// 충돌체크
	float AttackRadius = 500.f;

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		GetActorLocation(),
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(AttackRadius),
		Params
	);

	if (bResult)
	{
		for (auto OverlapResult : OverlapResults)
		{
			AHMonster* Monster = Cast<AHMonster>(OverlapResult.GetActor());

			if (Monster)
			{
				FDamageEvent DamageEvent;
				Monster->DamageAnim();
				if (CurrentGameMode == EGameMode::MULTI_GAME)
					Monster->MonsterDamageEffect(fSkillPower);
				else
					OverlapResult.Actor->TakeDamage(fSkillPower, DamageEvent, GetController(), this);
			}
		}
	}
}