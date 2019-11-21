// Fill out your copyright notice in the Description page of Project Settings.

#include "HVampAnimInstance.h"




UHVampAnimInstance::UHVampAnimInstance()
{
	CurrentPawnSpeed = 0.f;
	IsDead = false;

	// Montage
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_MONTAGE(TEXT("/Game/TowerofAngra/Monster/Vamp/Anim/Vamp_Montage.Vamp_Montage"));
	if (ATTACK_MONTAGE.Succeeded())
		AttackMontage = ATTACK_MONTAGE.Object;
}

void UHVampAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	if (::IsValid(Pawn))
	{
		CurrentPawnSpeed = Pawn->GetVelocity().Size();
	}
}

void UHVampAnimInstance::PlayAttackMontage()
{
	Montage_Play(AttackMontage, 1.f);
}

void UHVampAnimInstance::AnimNotify_AttackHitCheck()
{
	OnAttackHitCheck.Broadcast();
}

void UHVampAnimInstance::AnimNotify_NextAttackCheck()
{
}
