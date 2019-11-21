// Fill out your copyright notice in the Description page of Project Settings.

#include "HVamp.h"
#include "HCharaterStateComponent.h"
#include "Components/WidgetComponent.h"


AHVamp::AHVamp()
{
	CurrentMonsterType = EMonsterName::VAMP;

	BodyCenter = 88.f;
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CARDBOARD(TEXT("/Game/TowerofAngra/Monster/Vamp/vampire_a_lusth.vampire_a_lusth"));
	if (SK_CARDBOARD.Succeeded())
		GetMesh()->SetSkeletalMesh(SK_CARDBOARD.Object);

	GetMesh()->SetRelativeLocationAndRotation(FVector(-20.f, 0.f, - BodyCenter), FRotator(0.f, -90.f, 0.f));
	
	// Anim
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> GOLEM_ANIM(TEXT("/Game/TowerofAngra/Monster/Vamp/Anim/VampAnimBlueprint.VampAnimBlueprint_C"));
	if (GOLEM_ANIM.Succeeded())
		GetMesh()->SetAnimInstanceClass(GOLEM_ANIM.Class);

	CurrentHP = 100.f;
	CharacterState->InitHP(CurrentHP);

	// UI Pos
	HPBarWidget->SetupAttachment(GetMesh());
	HPBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 180.f));
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);

	DetectRadius = 800.f;
	AttackRadius = 50.f;
	AttackRange = 80.f;
	fAttackPower = 10.f;
}

