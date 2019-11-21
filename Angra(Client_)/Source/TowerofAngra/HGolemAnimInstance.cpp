// Fill out your copyright notice in the Description page of Project Settings.

#include "HGolemAnimInstance.h"

UHGolemAnimInstance::UHGolemAnimInstance()
{
	CurrentPawnSpeed = 0.f;
	IsDead = false;

	// Montage
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_MONTAGE(TEXT("/Game/TowerofAngra/Monster/Golem/Animations/SK_Golem_Skeleton_Montage.SK_Golem_Skeleton_Montage"));
	if (ATTACK_MONTAGE.Succeeded())
		AttackMontage = ATTACK_MONTAGE.Object;
}

void UHGolemAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	if (::IsValid(Pawn))
	{
		CurrentPawnSpeed = Pawn->GetVelocity().Size();
	}
}

void UHGolemAnimInstance::PlayAttackMontage()
{
	Montage_Play(AttackMontage, 1.f);
}

void UHGolemAnimInstance::AnimNotify_AttackHitCheck()
{
	OnAttackHitCheck.Broadcast();
}

void UHGolemAnimInstance::AnimNotify_NextAttackCheck()
{
}
