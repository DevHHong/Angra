// Fill out your copyright notice in the Description page of Project Settings.

#include "HKayaCharacter.h"
#include "HKayaAnimInstance.h"
#include "HMonster.h"
#include "HCharaterStateComponent.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AHKayaCharacter::AHKayaCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_KAYA(TEXT("/Game/TowerofAngra/Character/Kaya/Mesh/Kaya.Kaya"));
	if (SK_KAYA.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_KAYA.Object);
	}

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> KAYA_ANIM(TEXT("/Game/TowerofAngra/Character/Kaya/Animation/HKaya_AnimBlueprint.HKaya_AnimBlueprint_C"));
	if (KAYA_ANIM.Succeeded())
		GetMesh()->SetAnimInstanceClass(KAYA_ANIM.Class);

	InitCommon();			// 캐릭터 컨트롤 설정
	AttackEndComboState();

	//Set Particle
	static ConstructorHelpers::FObjectFinder<UParticleSystem> PARTICLE(TEXT("ParticleSystem'/Game/VFX_Toolkit_V1/ParticleSystems/356Days/Par_KineticForce_01.Par_KineticForce_01'"));
	SkillEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Effect"));

	if (PARTICLE.Succeeded())
	{
		SkillEffect->SetTemplate(PARTICLE.Object);
		SkillEffect->bAutoActivate = false;
	}

	SkillMP = 20.f;

	// Sound
	/*static ConstructorHelpers::FObjectFinder<USoundBase> JUMPSOUND(TEXT("/Game/HumanVocalizations/HumanFemaleB/Wavs/voice_female_b_effort_jump_action_10.voice_female_b_effort_jump_action_10"));
	if (JUMPSOUND.Succeeded())
		JumpSound = JUMPSOUND.Object;*/

	static ConstructorHelpers::FObjectFinder<USoundBase> SKILLSOUND(TEXT("/Game/HumanVocalizations/HumanFemaleC/Wavs/voice_female_c_battle_shout_short_02.voice_female_c_battle_shout_short_02"));
	if (SKILLSOUND.Succeeded())
		HowlSound = SKILLSOUND.Object;

}

// Called when the game starts or when spawned
void AHKayaCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AHKayaCharacter::InitCommon()
{
	Super::InitCommon();
	MaxCombo = 4;			// 콤보 개수
	fAttackPower = 50.f;			// 기본 공격력
	fSkillPower = 100.f;			// 스킬 공격력
}

// Called every frame
void AHKayaCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHKayaCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	KayaAnim = Cast<UHKayaAnimInstance>(GetMesh()->GetAnimInstance());
	if (KayaAnim == nullptr) return;

	KayaAnim->OnMontageEnded.AddDynamic(this, &AHKayaCharacter::OnAttackMontageEnded);
	KayaAnim->OnMontageEnded.AddDynamic(this, &AHKayaCharacter::OnSkillMontageEnded);
	

	KayaAnim->OnNextAttackCheck.AddLambda([this]() -> void {
		CanNextCombo = false;

		if (IsComboInputOn)
		{
			AttackStartComboState();
			KayaAnim->JumpToAttackMontageSection(CurrentCombo);
		}
	});

	KayaAnim->OnAttackHitCheck.AddUObject(this, &AHKayaCharacter::AttackCheck);
}

// Called to bind functionality to input
void AHKayaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AHKayaCharacter::Attack);

	PlayerInputComponent->BindAction(TEXT("Skill"), EInputEvent::IE_Pressed, this, &AHKayaCharacter::Skill);
}

void AHKayaCharacter::Attack()
{
	Super::Attack();

	if (IsSkilling) return;

	if (IsAttacking)
	{
		if (CanNextCombo)
			IsComboInputOn = true;
	}
	else
	{
		AttackStartComboState();
		KayaAnim->PlayAttackMontage();
		KayaAnim->JumpToAttackMontageSection(CurrentCombo);
		IsAttacking = true;
	}
}

void AHKayaCharacter::Skill()
{
	Super::Skill();

	if (IsSkilling) return;
	if (IsAttacking) return;
	if (CharacterState->GetMP() <= SkillMP)return;

	FinalMana += SkillMP;
	
	KayaAnim->PlaySkillMontage();
	SkillEffect->Activate(true);
	// Effect 위치 다시 받기
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SkillEffect->Template, GetActorLocation(), GetActorRotation());
	UGameplayStatics::PlaySoundAtLocation(this, HowlSound, GetActorLocation());
	SkillCheck();

	IsSkilling = true;
}

void AHKayaCharacter::OnAttackMontageEnded(UAnimMontage * Montage, bool bInterrupted)
{
	// 공격이 끝나는 모션
	IsAttacking = false;
	
	AttackEndComboState();
}

void AHKayaCharacter::AttackStartComboState()
{
	CanNextCombo = true;
	IsComboInputOn = false;

	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
}

void AHKayaCharacter::AttackEndComboState()
{
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentCombo = 0;
}

void AHKayaCharacter::AttackCheck()
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

	if (bResult)
	{
		if (HitResult.Actor.IsValid())
		{
			FDamageEvent DamageEvent;
			HitResult.Actor->TakeDamage(fAttackPower, DamageEvent, GetController(), this);
		}
	}
}

void AHKayaCharacter::OnSkillMontageEnded(UAnimMontage * Montage, bool bInterrupted)
{
	IsSkilling = false;

	OnSkillEnd.Broadcast();
}

void AHKayaCharacter::SkillCheck()
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
				OverlapResult.Actor->TakeDamage(fSkillPower, DamageEvent, GetController(), this);
			}
		}
	}
}

