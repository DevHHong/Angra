// Fill out your copyright notice in the Description page of Project Settings.

#include "HWarriorAnimInstance.h"


UHWarriorAnimInstance::UHWarriorAnimInstance()
{
	CurrentPawnSpeed = 0.f;
	IsInAir = false;
	IsDead = false;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_MONTAGE(TEXT("/Game/TowerofAngra/Character/Warrior/Animations/HWarriorAttack_Montage.HWarriorAttack_Montage"));
	if (ATTACK_MONTAGE.Succeeded())
		AttackMontage = ATTACK_MONTAGE.Object;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> SKILL_MONTAGE(TEXT("/Game/TowerofAngra/Character/Warrior/Animations/HWarriorSkill_Montage.HWarriorSkill_Montage"));
	if (SKILL_MONTAGE.Succeeded())
		SkillMontage = SKILL_MONTAGE.Object;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> SKILL_COMMON_MONTAGE(TEXT("/Game/TowerofAngra/Character/Warrior/Animations/HWarriorSkill3_Montage.HWarriorSkill3_Montage"));
	if (SKILL_COMMON_MONTAGE.Succeeded())
		CommonSkillMontage = SKILL_COMMON_MONTAGE.Object;
}

void UHWarriorAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
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

void UHWarriorAnimInstance::PlayAttackMontage()
{
	Montage_Play(AttackMontage, 1.f);
}

void UHWarriorAnimInstance::PlaySkillMontage()
{
	Montage_Play(SkillMontage, 1.5f);
}

void UHWarriorAnimInstance::PlayCommonSkillMontage()
{
	Montage_Play(CommonSkillMontage, 1.5f);
}

void UHWarriorAnimInstance::JumpToAttackMontageSection(int32 NewSection)
{
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), AttackMontage);
}

void UHWarriorAnimInstance::AnimNotify_AttackHitCheck()
{
	OnAttackHitCheck.Broadcast();
}

void UHWarriorAnimInstance::AnimNotify_NextAttackCheck()
{
	OnNextAttackCheck.Broadcast();
}

void UHWarriorAnimInstance::AnimNotify_CommonSkillCheck()
{
	OnCommonSkillCheck.Broadcast();
}

FName UHWarriorAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}
