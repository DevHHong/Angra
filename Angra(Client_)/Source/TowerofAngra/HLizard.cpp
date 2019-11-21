// Fill out your copyright notice in the Description page of Project Settings.

#include "HLizard.h"
#include "HCharaterStateComponent.h"
#include "Components/WidgetComponent.h"

AHLizard::AHLizard()
{
	CurrentMonsterType = EMonsterName::LIZARD;

	BodyCenter = 70.f;
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CARDBOARD(TEXT("/Game/TowerofAngra/Monster/LizardBeast/Meshes/SK_LizardBeast.SK_LizardBeast"));
	if (SK_CARDBOARD.Succeeded())
		GetMesh()->SetSkeletalMesh(SK_CARDBOARD.Object);
	GetCapsuleComponent()->SetCapsuleSize(50.f, BodyCenter, true);
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, - BodyCenter), FRotator(0.f, -90.f, 0.f));

	// Anim
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> LIZARD_ANIM(TEXT("/Game/TowerofAngra/Monster/LizardBeast/Animations/LizardAnimBlueprint.LizardAnimBlueprint_C"));
	if (LIZARD_ANIM.Succeeded())
		GetMesh()->SetAnimInstanceClass(LIZARD_ANIM.Class);

	CurrentHP = 300.f;
	CharacterState->InitHP(CurrentHP);

	// UI Pos
	HPBarWidget->SetupAttachment(GetMesh());
	HPBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 150.f));
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);

	// ¼Óµµ
	GetCharacterMovement()->MaxWalkSpeed = 500.f;

	DetectRadius = 1000;
	AttackRadius = 80.f;
	AttackRange = 100.f;
	fAttackPower = 15.f;
}