// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "TowerofAngraCharacter.h"
#include "HCharaterStateComponent.h"
#include "Components/WidgetComponent.h"
#include "HHUDWidget.h"
#include "HOtherPlayerHPWidget.h"
#include "TowerofAngraGameMode.h"
#include "HMonster.h"

//////////////////////////////////////////////////////////////////////////
// ATowerofAngraCharacter

ATowerofAngraCharacter::ATowerofAngraCharacter()
{
	/* ======= Server ======= */
	SessionId = -1;
	/* ======= Server ======= */

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("TOACharater"));			// 충돌체크할때 캐릭터의 이름을 찾는다.

	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));

	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);

	SetCamMode(ECAMMode::DYNAMIC_CAM);
	
	// 캐릭터 UI 및 상태 =======================
	// UI
	UI_Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBARWIDGET"));
	static ConstructorHelpers::FClassFinder<UUserWidget> UI(TEXT("/Game/TowerofAngra/UI/UI_HUD.UI_HUD_C"));
	if (UI.Succeeded())
	{
		UI_Widget->SetWidgetClass(UI.Class);
		UI_Widget->SetVisibility(false);
	}

	CharacterState = CreateDefaultSubobject<UHCharaterStateComponent>(TEXT("CHARACTERSTATE"));	// 캐릭터 상태

	CurrentHP = 100.f;
	CharacterState->InitHP(CurrentHP);
	
	// HpUI
	OtherPlayerHPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OTHERHPBARWIDGET"));

	static ConstructorHelpers::FClassFinder<UUserWidget> UI_OtherPlayerHPBar(TEXT("/Game/TowerofAngra/UI/UI_PlayerHpBar.UI_PlayerHpBar_C"));
	if (UI_OtherPlayerHPBar.Succeeded())
	{
		OtherPlayerHPBarWidget->SetWidgetClass(UI_OtherPlayerHPBar.Class);
		OtherPlayerHPBarWidget->SetDrawSize(FVector2D(150.f, 50.f));
		// UI Pos
		OtherPlayerHPBarWidget->SetupAttachment(GetMesh());
		OtherPlayerHPBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 180.f));
		OtherPlayerHPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
		//IsHP_Show = false;
	}
	
	// =========================================

	// 플레이어 카메라 무시
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);


	// Camera Shake
	static ConstructorHelpers::FClassFinder<UCameraShake> SHAKE(TEXT("/Script/TowerofAngra.HPlayerCameraShake"));
	if (SHAKE.Succeeded())
		CameraShake = SHAKE.Class;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> PARTICLE2(TEXT("ParticleSystem'/Game/VFX_Toolkit_V1/ParticleSystems/356Days/Par_MatraBoom_01.Par_MatraBoom_01'"));
	AttackDamageEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Effect2"));

	if (PARTICLE2.Succeeded())
	{
		AttackDamageEffect->SetTemplate(PARTICLE2.Object);
		AttackDamageEffect->bAutoActivate = false;
	}

	// Sound
	static ConstructorHelpers::FObjectFinder<USoundBase> EATSOUND(TEXT("/Game/VFX_Toolkit_V1/Sounds/S_Rocket_Launch_01.S_Rocket_Launch_01"));
	if (EATSOUND.Succeeded())
		EatSound = EATSOUND.Object;
}

void ATowerofAngraCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	auto OtherCharacterWidget = Cast<UHOtherPlayerHPWidget>(OtherPlayerHPBarWidget->GetUserWidgetObject());
	if (nullptr != OtherCharacterWidget)
	{
		OtherCharacterWidget->BindCharacterState(CharacterState);
	}

	if (!IsPlayerControlled()) return;

	// HUD
	auto CharacterWidget = Cast<UHHUDWidget>(UI_Widget->GetUserWidgetObject());

	if (nullptr != CharacterWidget)
	{
		CharacterWidget->AddToViewport(1);
		CharacterWidget->BindCharacterState(CharacterState);
	}
}

void ATowerofAngraCharacter::InitCommon()
{
	CurrentPlayerType = PlayerType;		// 내 캐릭 타입

	ArmLengthSpeed = 3.f;
	ArmRotationSpeed = 10.f;
	GetCharacterMovement()->JumpZVelocity = 700.f;	// 점프 높이
	FinalDamage = 0.f;
	FinalMana = 0.f;
	IsAttacking = false;
	IsSkilling = false;
	fAttackPower = 50.f;			// 기본 공격력
	fCommonSkillPower = 80.f;		// 공통 스킬공격력
	CommonSkillMP = 20.f;			// 공통 스킬 마나
	IsFillHP = false;				// 체력 채우는 판단
	IsFillMP = false;
	FillHP_Limit = 0.f;
	FillMP_Limit = 0.f;
	SKILL_TYPE = ESkillType::NONE;
}

//////////////////////////////////////////////////////////////////////////
// Input

void ATowerofAngraCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ATowerofAngraCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ATowerofAngraCharacter::MoveRight);

	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ATowerofAngraCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ATowerofAngraCharacter::Turn);

	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ATowerofAngraCharacter::Jump);

	PlayerInputComponent->BindAction(TEXT("ViewChange"), EInputEvent::IE_Pressed, this, &ATowerofAngraCharacter::ViewChange);
	PlayerInputComponent->BindAction(TEXT("Cheat"), EInputEvent::IE_Pressed, this, &ATowerofAngraCharacter::Cheat);

	//TOALOG_S(Warning);
}

void ATowerofAngraCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SpringArmTick(DeltaTime);

	StateTick(DeltaTime);

	CurrentHP = CharacterState->GetHP();
	//TOALOG(Warning, TEXT("CurrentHP : %f"), CurrentHP);
}

void ATowerofAngraCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void ATowerofAngraCharacter::SetCamMode(ECAMMode NewCamMode)
{
	CurrentCamMode = NewCamMode;
	switch (CurrentCamMode)
	{
	case ECAMMode::DYNAMIC_CAM:
		ArmLengthTo = 500.f;
		ArmRotationTo = FRotator(-15.f, 0.f, 0.f);
		SpringArm->bUsePawnControlRotation = true;
		SpringArm->bInheritPitch = true;		// 고정하려면 : false
		SpringArm->bInheritRoll = true;
		SpringArm->bInheritYaw = true;
		SpringArm->bDoCollisionTest = true;
		
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
		break;
	case ECAMMode::STATIC_CAM:
		ArmLengthTo = 800.f;
		ArmRotationTo = FRotator(-45.f, 0.f, 0.f);
		SpringArm->bUsePawnControlRotation = false;
		SpringArm->bInheritPitch = false;
		SpringArm->bInheritRoll = false;
		SpringArm->bInheritYaw = false;
		SpringArm->bDoCollisionTest = false;
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
		break;
	}
}

float ATowerofAngraCharacter::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	if (IsPlayerControlled())
		FinalDamage += Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), AttackDamageEffect->Template, GetActorLocation(), GetActorRotation());
	return FinalDamage;
}

void ATowerofAngraCharacter::SpringArmTick(float DeltaTime)
{
	/*SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, ArmLengthTo, DeltaTime, ArmLengthSpeed);

	switch (CurrentCamMode)
	{
	case ECAMMode::DYNAMIC_CAM:
	case ECAMMode::STATIC_CAM:
		SpringArm->RelativeRotation = FMath::RInterpTo(SpringArm->RelativeRotation, ArmRotationTo, DeltaTime, ArmRotationSpeed);
		break;
	}

	switch (CurrentCamMode)
	{
	case ECAMMode::DYNAMIC_CAM:
	case ECAMMode::STATIC_CAM:
		if (DirectionToMove.SizeSquared() > 0.f)
		{
			GetController()->SetControlRotation(FRotationMatrix::MakeFromX(DirectionToMove).Rotator());
			AddMovementInput(DirectionToMove);
		}
		break;
	}*/

	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, ArmLengthTo, DeltaTime, ArmLengthSpeed);

	switch (CurrentCamMode)
	{
	case  ECAMMode::STATIC_CAM:
		SpringArm->RelativeRotation = FMath::RInterpTo(SpringArm->RelativeRotation, ArmRotationTo, DeltaTime, ArmRotationSpeed);
		break;
	}

	switch (CurrentCamMode)
	{
	case  ECAMMode::STATIC_CAM:
		if (DirectionToMove.SizeSquared() > 0.f)
		{
			GetController()->SetControlRotation(FRotationMatrix::MakeFromX(DirectionToMove).Rotator());
			AddMovementInput(DirectionToMove);
		}
		break;
	}
}

void ATowerofAngraCharacter::StateTick(float DeltaTime)
{
	// 마나 채우기
	CharacterState->SetMP_FILL(DeltaTime);

	// HP 데미지
	if (FinalDamage > 0.f)
	{
		float DamageSpeed = DeltaTime * 40.f;
		CharacterState->SetDamage(DamageSpeed);
		FinalDamage -= DamageSpeed;
	}
	else
		FinalDamage = 0.f;
	// 스킬 사용시 마나 감소
	if (FinalMana > 0.f)
	{
		float ManaSpeed = DeltaTime * 50.f;
		CharacterState->SetMP(ManaSpeed);
		FinalMana -= ManaSpeed;
	}
	else
		FinalMana = 0.f;

	if (IsFillHP)
	{
		float fillSpeed = DeltaTime * 90.f;
		CharacterState->SetHP_FILL(fillSpeed);

		FillHP_Limit += fillSpeed;
		
		if (CurrentHP >= 100.f)
		{
			FillHP_Limit = 0.f;
			IsFillHP = false;
			CurrentHP = 100.f;
			CharacterState->SetHP(CurrentHP);
		}
		else if (FillHP_Limit >= 20.f)
		{
			FillHP_Limit = 0.f;
			IsFillHP = false;
		}
		//TOALOG(Warning, TEXT("FillHP_Limit : %f"), FillHP_Limit);
	}

	if (IsFillMP)
	{
		float fillSpeed = DeltaTime * 90.f;
		CharacterState->SetMP_FILL(fillSpeed);

		FillMP_Limit += fillSpeed;

		if (CharacterState->GetMP() >= 100.f)
		{
			FillMP_Limit = 0.f;
			IsFillMP = false;
			
			CharacterState->SetMPChange(100.f);
		}
		else if (FillMP_Limit >= 20.f)
		{
			FillMP_Limit = 0.f;
			IsFillMP = false;
		}
	}
}

void ATowerofAngraCharacter::Attack()
{
}

void ATowerofAngraCharacter::OtherPlayerAttack(int AttackCount)
{
}

void ATowerofAngraCharacter::Skill()
{
	SKILL_TYPE = ESkillType::SKILL;
}

void ATowerofAngraCharacter::CommonSkill()
{
	SKILL_TYPE = ESkillType::COMMONSKILL;
}

void ATowerofAngraCharacter::CommonSkillCheck()
{
	if (CameraShake != NULL)
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(CameraShake, 1.f);

	// 충돌체크
	float AttackRadius = 300.f;
	
	FVector SkillPos = GetActorLocation() + GetActorForwardVector() * 500.f;		// 스킬 떨어지는 위치
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		SkillPos,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(AttackRadius),
		Params
	);

#if ENABLE_DRAW_DEBUG
	DrawDebugSphere(GetWorld(), SkillPos, AttackRadius, 16, FColor::Green, false, 2.0f);

#endif
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
					Monster->MonsterDamageEffect(fCommonSkillPower);
				else
					OverlapResult.Actor->TakeDamage(fCommonSkillPower, DamageEvent, GetController(), this);
			}
		}
	}
}

void ATowerofAngraCharacter::ServerSetHP(float hp)
{
	CharacterState->SetHP(hp);
}

void ATowerofAngraCharacter::WeaponChange()
{
	Weapon->SetStaticMesh(SubWeapon->GetStaticMesh());
	UGameplayStatics::PlaySoundAtLocation(this, EatSound, GetActorLocation());
	fAttackPower = 100.f;
}

void ATowerofAngraCharacter::ServerRecvSkillCheck(ESkillType skill_type)
{
	if (skill_type == ESkillType::COMMONSKILL)
		CommonSkill();
	else if (skill_type == ESkillType::SKILL)
		Skill();
}

void ATowerofAngraCharacter::MoveForward(float NewAxisValue)
{
	if (IsSkilling) return;
	if (IsAttacking) return;
	switch (CurrentCamMode)
	{
	case ECAMMode::DYNAMIC_CAM:
		AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X), NewAxisValue);
		break;
	case ECAMMode::STATIC_CAM:
		DirectionToMove.X = NewAxisValue;
		break;
	}
}

void ATowerofAngraCharacter::MoveRight(float NewAxisValue)
{
	if (IsSkilling) return;
	if (IsAttacking) return;
	switch (CurrentCamMode)
	{
	case ECAMMode::DYNAMIC_CAM:
		AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y), NewAxisValue);
		break;
	case ECAMMode::STATIC_CAM:
		DirectionToMove.Y = NewAxisValue;
		break;
	}
}

void ATowerofAngraCharacter::LookUp(float NewAxisValue)
{
	switch (CurrentCamMode)
	{
	case ECAMMode::DYNAMIC_CAM:
		AddControllerPitchInput(NewAxisValue);
		break;
	}
}

void ATowerofAngraCharacter::Turn(float NewAxisValue)
{
	switch (CurrentCamMode)
	{
	case ECAMMode::DYNAMIC_CAM:
		AddControllerYawInput(NewAxisValue);
		break;
	}
}

void ATowerofAngraCharacter::Jump()
{
	if (IsSkilling) return;
	Super::Jump();
	UGameplayStatics::PlaySoundAtLocation(this, JumpSound, GetActorLocation());
}

void ATowerofAngraCharacter::ViewChange()
{
	switch (CurrentCamMode)
	{
	case ECAMMode::DYNAMIC_CAM:
		//GetController()->SetControlRotation(GetActorRotation());
		SetCamMode(ECAMMode::STATIC_CAM);
		break;
	case ECAMMode::STATIC_CAM:
		GetController()->SetControlRotation(SpringArm->RelativeRotation);
		SetCamMode(ECAMMode::DYNAMIC_CAM);
		break;
	}

	auto GameMode = Cast<ATowerofAngraGameMode>(GetWorld()->GetAuthGameMode());

	GameMode->AddScore();
}

void ATowerofAngraCharacter::Cheat()
{
	CurrentScore += 17;
	FinalDamage = 10;
}
