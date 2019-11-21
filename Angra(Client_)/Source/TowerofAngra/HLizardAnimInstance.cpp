// Fill out your copyright notice in the Description page of Project Settings.

#include "HLizardAnimInstance.h"



UHLizardAnimInstance::UHLizardAnimInstance()
{
	CurrentPawnSpeed = 0.f;
	IsDead = false;

	// Montage
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_MONTAGE(TEXT("/Game/TowerofAngra/Monster/LizardBeast/Animations/LizardAttack1_AnimMontage.LizardAttack1_AnimMontage"));
	if (ATTACK_MONTAGE.Succeeded())
		AttackMontage = ATTACK_MONTAGE.Object;
}

void UHLizardAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	if (::IsValid(Pawn))
	{
		CurrentPawnSpeed = Pawn->GetVelocity().Size();
	}
}

void UHLizardAnimInstance::PlayAttackMontage()
{
	Montage_Play(AttackMontage, 1.f);
}

void UHLizardAnimInstance::AnimNotify_AttackHitCheck()
{
	OnAttackHitCheck.Broadcast();
}

void UHLizardAnimInstance::AnimNotify_NextAttackCheck()
{
}
