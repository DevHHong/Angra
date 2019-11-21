// Fill out your copyright notice in the Description page of Project Settings.

#include "HAxeCharacter.h"
#include "HAxeAnimInstance.h"
#include "HCharaterStateComponent.h"
#include "HMonster.h"

AHAxeCharacter::AHAxeCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_AXE(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Ram.SK_CharM_Ram"));
	if (SK_AXE.Succeeded())
		GetMesh()->SetSkeletalMesh(SK_AXE.Object);

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> AXE_ANIM(TEXT("/Game/TowerofAngra/Character/AxeChar/Animations/HAxe_AnimBlueprint.HAxe_AnimBlueprint_C"));
	if (AXE_ANIM.Succeeded())
		GetMesh()->SetAnimInstanceClass(AXE_ANIM.Class);

	// Weapon
	FName WeaponSocket(TEXT("hand_rSocket"));
	if (GetMesh()->DoesSocketExist(WeaponSocket))
	{
		Weapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WEAPON"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_WEAPON(TEXT("/Game/InfinityBladeWeapons/Weapons/Blade/Axes/Blade_FissureBlade/SM_FissureBlade.SM_FissureBlade"));

		if (SM_WEAPON.Succeeded())
		{
			Weapon->SetStaticMesh(SM_WEAPON.Object);
		}

		Weapon->SetupAttachment(GetMesh(), WeaponSocket);
	}

	// SubWeapon
	SubWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SUBWEAPON"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_SUBWEAPON(TEXT("/Game/InfinityBladeWeapons/Weapons/Blade/Axes/Blade_TAxe/SM_Blade_TAxe.SM_Blade_TAxe"));
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

void AHAxeCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AHAxeCharacter::InitCommon()
{
	Super::InitCommon();
	MaxCombo = 4;			// 콤보 개수

	fAttackPower = 50.f;			// 기본 공격력
	fSkillPower = 150.f;			// 스킬 공격력
	SkillMP = 30.f;					// 마나 소모

	IsServerSend_Attacking = false;
	AttackEndComboState();
}

void AHAxeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHAxeCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AxeAnim = Cast<UHAxeAnimInstance>(GetMesh()->GetAnimInstance());
	if (AxeAnim == nullptr) return;

	AxeAnim->OnMontageEnded.AddDynamic(this, &AHAxeCharacter::OnAttackMontageEnded);
	AxeAnim->OnMontageEnded.AddDynamic(this, &AHAxeCharacter::OnSkillMontageEnded);

	AxeAnim->OnNextAttackCheck.AddLambda([this]() -> void {
		CanNextCombo = false;

		if (IsComboInputOn)
		{
			AttackStartComboState();
			AxeAnim->JumpToAttackMontageSection(CurrentCombo);
		}
	});

	AxeAnim->OnAttackHitCheck.AddUObject(this, &AHAxeCharacter::AttackCheck);
	AxeAnim->OnSkillHitCheck.AddUObject(this, &AHAxeCharacter::SkillCheck);
	AxeAnim->OnCommonSkillCheck.AddUObject(this, &AHAxeCharacter::CommonSkillCheck);
}

void AHAxeCharacter::SetupPlayerInputComponent(UInputComponent * PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AHAxeCharacter::Attack);
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Released, this, &AHAxeCharacter::NotAttack);

	PlayerInputComponent->BindAction(TEXT("Skill"), EInputEvent::IE_Pressed, this, &AHAxeCharacter::Skill);
	PlayerInputComponent->BindAction(TEXT("CommonSkill"), EInputEvent::IE_Pressed, this, &AHAxeCharacter::CommonSkill);
}

void AHAxeCharacter::Attack()
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
		AxeAnim->PlayAttackMontage();
		AxeAnim->JumpToAttackMontageSection(CurrentCombo);
		IsAttacking = true;
	}
}

void AHAxeCharacter::Skill()
{
	Super::Skill();

	if (IsSkilling) return;
	if (IsAttacking) return;
	if (CharacterState->GetMP() <= SkillMP)return;

	FinalMana += SkillMP;

	AxeAnim->PlaySkillMontage();
	
	IsSkilling = true;
}

void AHAxeCharacter::CommonSkill()
{
	Super::CommonSkill();

	if (IsSkilling) return;
	if (IsAttacking) return;
	if (CharacterState->GetMP() <= CommonSkillMP)return;

	FinalMana += CommonSkillMP;
	AxeAnim->PlayCommonSkillMontage();
	IsSkilling = true;
}

void AHAxeCharacter::OtherPlayerAttack(int AttackCount)
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
		AxeAnim->PlayAttackMontage();
		AxeAnim->JumpToAttackMontageSection(CurrentCombo);
		IsAttacking = true;
	}
}

void AHAxeCharacter::NotAttack()
{
	IsServerSend_Attacking = false;
}

void AHAxeCharacter::OnAttackMontageEnded(UAnimMontage * Montage, bool bInterrupted)
{
	IsAttacking = false;
	IsServerSend_Attacking = false;
	AttackEndComboState();
}

void AHAxeCharacter::AttackStartComboState()
{
	CanNextCombo = true;
	IsComboInputOn = false;

	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
}

void AHAxeCharacter::AttackEndComboState()
{
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentCombo = 0;
}

void AHAxeCharacter::AttackCheck()
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

	/*FVector TraceVec = GetActorForwardVector() * FinalAttackRange;
	FVector Center = GetActorLocation() + TraceVec * 0.5f;
	float HalfHeight = FinalAttackRange * 0.5f + AttackRadius;
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	float DebugLifeTime = 3.f;

	DrawDebugCapsule(GetWorld(),
		Center,
		HalfHeight,
		AttackRadius,
		CapsuleRot,
		DrawColor,
		false,
		DebugLifeTime);*/

#endif
	AHMonster* Monster = Cast<AHMonster>(HitResult.Actor);
	if (bResult && Monster)
	{
		if (HitResult.Actor.IsValid())
		{
			FDamageEvent DamageEvent;
			if (CurrentGameMode == EGameMode::MULTI_GAME)
			{
				Monster->MonsterDamageEffect(fAttackPower);
			}
			else
				HitResult.Actor->TakeDamage(fAttackPower, DamageEvent, GetController(), this);
		}
	}
}

void AHAxeCharacter::OnSkillMontageEnded(UAnimMontage * Montage, bool bInterrupted)
{
	IsSkilling = false;
	SKILL_TYPE = ESkillType::NONE;
	OnSkillEnd.Broadcast();
}

void AHAxeCharacter::SkillCheck()
{
	if (CameraShake != NULL)
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(CameraShake, 1.f);

	// 충돌체크
	float AttackRadius = 300.f;

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
