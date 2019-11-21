// Fill out your copyright notice in the Description page of Project Settings.

#include "HAxeAnimInstance.h"



UHAxeAnimInstance::UHAxeAnimInstance()
{
	CurrentPawnSpeed = 0.f;
	IsInAir = false;
	IsDead = false;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_MONTAGE(TEXT("/Game/TowerofAngra/Character/AxeChar/Animations/HAxeAttack_Montage.HAxeAttack_Montage"));
	if (ATTACK_MONTAGE.Succeeded())
		AttackMontage = ATTACK_MONTAGE.Object;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> SKILL_MONTAGE(TEXT("/Game/TowerofAngra/Character/AxeChar/Animations/HAxeSkill_Montage.HAxeSkill_Montage"));
	if (SKILL_MONTAGE.Succeeded())
		SkillMontage = SKILL_MONTAGE.Object;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> SKILL_COMMON_MONTAGE(TEXT("/Game/TowerofAngra/Character/Warrior/Animations/HWarriorSkill3_Montage.HWarriorSkill3_Montage"));
	if (SKILL_COMMON_MONTAGE.Succeeded())
		CommonSkillMontage = SKILL_COMMON_MONTAGE.Object;
}

void UHAxeAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	if (::IsValid(Pawn))
	{
		CurrentPawnSpeed = Pawn->GetVelocity().Size();
		// Jump
		auto Character = Cast<ACharacter>(Pawn);
		if (Character)
		{
			IsInAir = Character->GetMovementComponent()->IsFalling();
		}
	}
}

void UHAxeAnimInstance::PlayAttackMontage()
{
	Montage_Play(AttackMontage, 1.f);
}

void UHAxeAnimInstance::PlaySkillMontage()
{
	Montage_Play(SkillMontage, 1.f);
}

void UHAxeAnimInstance::PlayCommonSkillMontage()
{
	Montage_Play(CommonSkillMontage, 1.5f);
}

void UHAxeAnimInstance::JumpToAttackMontageSection(int32 NewSection)
{
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), AttackMontage);
}

void UHAxeAnimInstance::AnimNotify_AttackHitCheck()
{
	OnAttackHitCheck.Broadcast();
}

void UHAxeAnimInstance::AnimNotify_NextAttackCheck()
{
	OnNextAttackCheck.Broadcast();
}

void UHAxeAnimInstance::AnimNotify_SkillHitCheck()
{
	OnSkillHitCheck.Broadcast();
}

void UHAxeAnimInstance::AnimNotify_CommonSkillCheck()
{
	OnCommonSkillCheck.Broadcast();
}

FName UHAxeAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}
