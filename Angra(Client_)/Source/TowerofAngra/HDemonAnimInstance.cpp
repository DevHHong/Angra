// Fill out your copyright notice in the Description page of Project Settings.

#include "HDemonAnimInstance.h"


UHDemonAnimInstance::UHDemonAnimInstance()
{
	CurrentPawnSpeed = 0.f;
	IsDead = false;

	// Montage
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_MONTAGE(TEXT("/Game/TowerofAngra/Demon/Anim/FireImpact.FireImpact"));
	if (ATTACK_MONTAGE.Succeeded())
		AttackMontage = ATTACK_MONTAGE.Object;
}

void UHDemonAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	if (::IsValid(Pawn))
	{
		CurrentPawnSpeed = Pawn->GetVelocity().Size();
	}
}

void UHDemonAnimInstance::PlayAttackMontage()
{
	Montage_Play(AttackMontage, 1.f);
}

void UHDemonAnimInstance::AnimNotify_AttackHitCheck()
{
	OnAttackHitCheck.Broadcast();
}

void UHDemonAnimInstance::AnimNotify_NextAttackCheck()
{
}
