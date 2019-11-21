// Fill out your copyright notice in the Description page of Project Settings.

#include "HMonster.h"
#include "HMonsterAIController.h"
#include "HGolemAnimInstance.h"
#include "HVampAnimInstance.h"
#include "HDemonAnimInstance.h"
#include "HLizardAnimInstance.h"
#include "HCharaterStateComponent.h"
#include "HMonsterHPWidget.h"
#include "HDropItem.h"
#include "TowerofAngraCharacter.h"
#include "Components/WidgetComponent.h"
#include "HTOAGameState.h"
#include "CharacterPlayerController.h"	

// Sets default values
AHMonster::AHMonster()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CharacterState = CreateDefaultSubobject<UHCharaterStateComponent>(TEXT("CHARACTERSTATE"));	// 캐릭터 상태
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("TOACharater"));			// 충돌체크할때 캐릭터의 이름을 찾는다.

	// HpUI
	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBARWIDGET"));
	
	static ConstructorHelpers::FClassFinder<UUserWidget> UI_MonsterHPBar(TEXT("/Game/TowerofAngra/UI/UI_MonsterHpBar.UI_MonsterHpBar_C"));
	if (UI_MonsterHPBar.Succeeded())
	{
		HPBarWidget->SetWidgetClass(UI_MonsterHPBar.Class);
		HPBarWidget->SetDrawSize(FVector2D(150.f, 50.f));
		IsHP_Show = false;		
	}

	AIControllerClass = AHMonsterAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	IsAttacking = false;
	IsServerAttacking = false;		// 서버로 부터 공격판단.
	AnimInstance = nullptr;
	DeadTimer = 3.f;
	FinalDamage = 0.f;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 480.f, 0.f);

	//Set Particle
	
	static ConstructorHelpers::FObjectFinder<UParticleSystem> PARTICLE(TEXT("ParticleSystem'/Game/VFX_Toolkit_V1/ParticleSystems/356Days/Par_FlashRing.Par_FlashRing'"));
	DamageEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Effect"));

	if (PARTICLE.Succeeded())
	{
		DamageEffect->SetTemplate(PARTICLE.Object);
		DamageEffect->bAutoActivate = false;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> PARTICLE2(TEXT("ParticleSystem'/Game/VFX_Toolkit_V1/ParticleSystems/356Days/Par_MatraBoom_01.Par_MatraBoom_01'"));
	AttackDamageEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Effect2"));

	if (PARTICLE2.Succeeded())
	{
		AttackDamageEffect->SetTemplate(PARTICLE2.Object);
		AttackDamageEffect->bAutoActivate = false;
	}

	//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);

	// 플레이어 카메라 무시
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);


	IsAliving = true;
	IsDie = false;
}

// Called when the game starts or when spawned
void AHMonster::BeginPlay()
{
	Super::BeginPlay();

	MonsterAIController = Cast<AHMonsterAIController>(GetController());


	if (CurrentGameMode == EGameMode::SINGLE_GAME)
		MonsterAIController->RunAI();

	//TOALOG(Warning, TEXT("Location X : %.3f"), GetActorLocation().X);
}

void AHMonster::DieOn()
{
	SetActorEnableCollision(false);
	SetActorHiddenInGame(false);
	HPBarWidget->SetHiddenInGame(false);
	MonsterAIController->StopAI();
	CurrentScore += 10;
	IsDie = true;

	GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda([this]()->void {

		float PosZ = GetActorLocation().Z;
		PosZ = (PosZ - BodyCenter) + 30; // 30 item z 크기
		GetWorld()->SpawnActor<AHDropItem>(FVector(GetActorLocation().X, GetActorLocation().Y, PosZ), FRotator::ZeroRotator);
		Destroy();
	}), DeadTimer, false);
}

// Called every frame
void AHMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 몬스터 체력 바
	HPBarWidget->SetVisibility(IsHP_Show);

	// HP 데미지
	if (FinalDamage > 0.f)
	{
		float DamageSpeed = DeltaTime * 150.f;
		CharacterState->SetDamage(DamageSpeed);
		FinalDamage -= DamageSpeed;
	}

	CurrentHP = CharacterState->GetHP();

	ServerHpShowTick();
}

void AHMonster::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	

	switch (CurrentMonsterType)
	{
	case EMonsterName::GOLEM:
	{
		AnimInstance = Cast<UHGolemAnimInstance>(GetMesh()->GetAnimInstance());
		if (nullptr == AnimInstance)return;

		AnimInstance->OnMontageEnded.AddDynamic(this, &AHMonster::OnAttackMontageEnded);
		CharacterState->OnHPIsZero.AddLambda([this]() -> void {
			dynamic_cast<UHGolemAnimInstance*>(AnimInstance)->SetDeadAnim();
			DieOn();
		});

		dynamic_cast<UHGolemAnimInstance*>(AnimInstance)->OnAttackHitCheck.AddUObject(this, &AHMonster::AttackCheck);
	}
	break;
	case EMonsterName::VAMP:
	{
		AnimInstance = Cast<UHVampAnimInstance>(GetMesh()->GetAnimInstance());
		if (nullptr == AnimInstance)return;

		AnimInstance->OnMontageEnded.AddDynamic(this, &AHMonster::OnAttackMontageEnded);

		CharacterState->OnHPIsZero.AddLambda([this]() -> void {
			dynamic_cast<UHVampAnimInstance*>(AnimInstance)->SetDeadAnim();
			DieOn();
		});

		dynamic_cast<UHVampAnimInstance*>(AnimInstance)->OnAttackHitCheck.AddUObject(this, &AHMonster::AttackCheck);
	}
	break;
	case EMonsterName::DEMON:
	{
		AnimInstance = Cast<UHDemonAnimInstance>(GetMesh()->GetAnimInstance());
		if (nullptr == AnimInstance)return;

		AnimInstance->OnMontageEnded.AddDynamic(this, &AHMonster::OnAttackMontageEnded);

		CharacterState->OnHPIsZero.AddLambda([this]() -> void {
			dynamic_cast<UHDemonAnimInstance*>(AnimInstance)->SetDeadAnim();
			DieOn();
		});

		dynamic_cast<UHDemonAnimInstance*>(AnimInstance)->OnAttackHitCheck.AddUObject(this, &AHMonster::SkillCheck);
	}
	break;
	case EMonsterName::LIZARD:
	{
		AnimInstance = Cast<UHLizardAnimInstance>(GetMesh()->GetAnimInstance());
		if (nullptr == AnimInstance)return;

		AnimInstance->OnMontageEnded.AddDynamic(this, &AHMonster::OnAttackMontageEnded);

		CharacterState->OnHPIsZero.AddLambda([this]() -> void {
			dynamic_cast<UHLizardAnimInstance*>(AnimInstance)->SetDeadAnim();
			DieOn();
		});

		dynamic_cast<UHLizardAnimInstance*>(AnimInstance)->OnAttackHitCheck.AddUObject(this, &AHMonster::AttackCheck);
	}
	break;
	default:
		break;
	}

	auto CharacterWidget = Cast<UHMonsterHPWidget>(HPBarWidget->GetUserWidgetObject());
	if (nullptr != CharacterWidget)
	{
		CharacterWidget->BindCharacterState(CharacterState);
	}
}

float AHMonster::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	FinalDamage += Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), AttackDamageEffect->Template, GetActorLocation(), GetActorRotation());
	return FinalDamage;
}

void AHMonster::PossessedBy(AController * NewController)
{
	Super::PossessedBy(NewController);
}

// Called to bind functionality to input
void AHMonster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AHMonster::Attack()
{
	if (IsAttacking) return;

	switch (CurrentMonsterType)
	{
	case EMonsterName::GOLEM:
	{
		AnimInstance = Cast<UHGolemAnimInstance>(GetMesh()->GetAnimInstance());
		if (nullptr == AnimInstance)return;

		dynamic_cast<UHGolemAnimInstance*>(AnimInstance)->PlayAttackMontage();
	}
	break;
	case EMonsterName::VAMP:
	{
		AnimInstance = Cast<UHVampAnimInstance>(GetMesh()->GetAnimInstance());
		if (nullptr == AnimInstance)return;

		dynamic_cast<UHVampAnimInstance*>(AnimInstance)->PlayAttackMontage();
	}
	break;
	case EMonsterName::DEMON:
	{
		AnimInstance = Cast<UHDemonAnimInstance>(GetMesh()->GetAnimInstance());
		if (nullptr == AnimInstance)return;

		dynamic_cast<UHDemonAnimInstance*>(AnimInstance)->PlayAttackMontage();
	}
	break;
	case EMonsterName::LIZARD:
	{
		AnimInstance = Cast<UHLizardAnimInstance>(GetMesh()->GetAnimInstance());
		if (nullptr == AnimInstance)return;

		dynamic_cast<UHLizardAnimInstance*>(AnimInstance)->PlayAttackMontage();
	}
	break;
	default:
		break;
	}

	IsAttacking = true;
}

void AHMonster::ServerAttack(EMonsterName MonsterType)
{
	if (IsServerAttacking) return;
	if (CharacterState->GetHP() <= 0) return;

	switch (MonsterType)
	{
	case EMonsterName::GOLEM:
	{
		AnimInstance = Cast<UHGolemAnimInstance>(GetMesh()->GetAnimInstance());
		if (nullptr == AnimInstance)return;

		dynamic_cast<UHGolemAnimInstance*>(AnimInstance)->PlayAttackMontage();
	}
	break;
	case EMonsterName::VAMP:
	{
		AnimInstance = Cast<UHVampAnimInstance>(GetMesh()->GetAnimInstance());
		if (nullptr == AnimInstance)return;

		dynamic_cast<UHVampAnimInstance*>(AnimInstance)->PlayAttackMontage();
	}
	break;
	case EMonsterName::DEMON:
	{
		AnimInstance = Cast<UHDemonAnimInstance>(GetMesh()->GetAnimInstance());
		if (nullptr == AnimInstance)return;

		dynamic_cast<UHDemonAnimInstance*>(AnimInstance)->PlayAttackMontage();
	}
	break;
	case EMonsterName::LIZARD:
	{
		AnimInstance = Cast<UHLizardAnimInstance>(GetMesh()->GetAnimInstance());
		if (nullptr == AnimInstance)return;

		dynamic_cast<UHLizardAnimInstance*>(AnimInstance)->PlayAttackMontage();
	}
	break;
	default:
		break;
	}
	IsServerAttacking = true;
}

void AHMonster::DamageAnim()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DamageEffect->Template, GetActorLocation(), GetActorRotation());
}

void AHMonster::OnAttackMontageEnded(UAnimMontage * Montage, bool bInterrupted)
{
	IsAttacking = false;
	IsServerAttacking = false;
	OnAttackEnd.Broadcast();
}

void AHMonster::AttackCheck()
{
	//FHitResult HitResult;
	
	//FCollisionQueryParams Params(NAME_None, false, this);
	//bool bResult = GetWorld()->SweepSingleByChannel(
	//	HitResult,
	//	GetActorLocation(),
	//	GetActorLocation() + GetActorForwardVector() * AttackRange,
	//	FQuat::Identity,
	//	ECollisionChannel::ECC_GameTraceChannel2,
	//	FCollisionShape::MakeSphere(AttackRadius),
	//	Params
	//);

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		GetActorLocation() + GetActorForwardVector() * AttackRange,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(AttackRadius),
		Params
	);
#if ENABLE_DRAW_DEBUG

	FVector TraceVec = GetActorForwardVector() * AttackRange;
	FVector Center = GetActorLocation() + TraceVec * 0.5f;
	float HalfHeight = AttackRange * 0.5f + AttackRadius;
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
		DebugLifeTime);

#endif

	/*ATowerofAngraCharacter* Player = Cast<ATowerofAngraCharacter>(HitResult.Actor);

	if (bResult && Player)
	{
		if (HitResult.Actor.IsValid())
		{
			FDamageEvent DamageEvent;
			HitResult.Actor->TakeDamage(fAttackPower, DamageEvent, GetController(), this);
		}
	}*/

	if (bResult)
	{
		for (auto OverlapResult : OverlapResults)
		{
			ATowerofAngraCharacter* Player = Cast<ATowerofAngraCharacter>(OverlapResult.GetActor());
			if (Player)
			{
				FDamageEvent DamageEvent;

				OverlapResult.Actor->TakeDamage(fAttackPower, DamageEvent, GetController(), this);
			}
		}
	}
}

void AHMonster::SkillCheck()
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
			ATowerofAngraCharacter* Player = Cast<ATowerofAngraCharacter>(OverlapResult.GetActor());
			if (Player)
			{
				FDamageEvent DamageEvent;
				
				OverlapResult.Actor->TakeDamage(fAttackPower, DamageEvent, GetController(), this);
			}
		}
	}
}

void AHMonster::MoveToLocation(const FVector & dest)
{
	if (CharacterState->GetHP() <= 0) return;
	AHMonsterAIController* Controller = Cast<AHMonsterAIController>(GetController());
	if (Controller)
	{
		Controller->MoveToLocation(dest);
	}
}

void AHMonster::ServerSendDieOn(EMonsterName MonsterType)
{
	CharacterState->SetHP(0.f);
}

void AHMonster::MonsterDamageEffect(const float& damage)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), AttackDamageEffect->Template, GetActorLocation(), GetActorRotation());

	ACharacterPlayerController* PlayerController = Cast<ACharacterPlayerController>(GetWorld()->GetFirstPlayerController());

	CharacterState->SetDamage(damage);

	PlayerController->HitMonster(MonsterID, CharacterState->GetHP(), damage, IsDie);

	PlayerController->NextStageHitMonster(MonsterID, CharacterState->GetHP(), damage, IsDie);

	if (CharacterState->GetHP() <= 0)
	{
		IsDie = true;
		CharacterState->SetHP(0);
	}
}

void AHMonster::ServerHpShowTick()
{
	if (CurrentGameMode != EGameMode::MULTI_GAME) return;

	if (nullptr == GetWorld()) return;
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, GetController());
	bool bResult = GetWorld()->OverlapMultiByChannel(
		OverlapResults, GetActorLocation(),
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(DetectRadius),
		CollisionQueryParam
	);

	// Player Target 했을시.
	if (bResult)
	{
		for (auto OverlapResult : OverlapResults)
		{
			ATowerofAngraCharacter* APlayer = Cast< ATowerofAngraCharacter>(OverlapResult.GetActor());

			if (APlayer && APlayer->GetController()->IsPlayerController())
			{
				IsHP_Show = true;
				return;
			}
		}
	}

	IsHP_Show = false;
}
