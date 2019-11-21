// Fill out your copyright notice in the Description page of Project Settings.

#include "HCharaterStateComponent.h"


// Sets default values for this component's properties
UHCharaterStateComponent::UHCharaterStateComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHCharaterStateComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UHCharaterStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UHCharaterStateComponent::InitHP(float NewHp)
{
	MaxHp = NewHp;
	SetHP(MaxHp);

	// 마나 초기화
	MaxMp = CurrentMP = 100.f;
}

void UHCharaterStateComponent::SetDamage(float NewDamage)
{
	SetHP(FMath::Clamp<float>(CurrentHP - NewDamage, 0.f, MaxHp));
}

void UHCharaterStateComponent::SetHP(float NewHP)
{
	CurrentHP = NewHP;
	OnHPChanged.Broadcast();
	if (CurrentHP < KINDA_SMALL_NUMBER)
	{
		CurrentHP = 0.f;
		OnHPIsZero.Broadcast();
	}
}

void UHCharaterStateComponent::SetMP(float NewMP)
{
	CurrentMP -= NewMP;
	OnMPChanged.Broadcast();
}

void UHCharaterStateComponent::SetMPChange(float NewMP)
{
	CurrentMP = NewMP;
	OnMPChanged.Broadcast();
	if (CurrentMP < KINDA_SMALL_NUMBER)
	{
		CurrentMP = 0.f;
	}
}

void UHCharaterStateComponent::SetHP_FILL(float NewHP)
{
	if (CurrentHP >= MaxHp)
		return;
	CurrentHP += NewHP;
	OnHPChanged.Broadcast();
}

void UHCharaterStateComponent::SetMP_FILL(float NewMP)
{
	if (CurrentMP >= MaxMp)
		return;
	CurrentMP += NewMP;
	OnMPChanged.Broadcast();
}

float UHCharaterStateComponent::GetHPRatio() const
{
	return (MaxHp < KINDA_SMALL_NUMBER) ? 0.f : (CurrentHP / MaxHp);
}

float UHCharaterStateComponent::GetMPRatio() const
{
	return (MaxMp < KINDA_SMALL_NUMBER) ? 0.f : (CurrentMP / MaxMp);
}

float UHCharaterStateComponent::GetHP() const
{
	return CurrentHP;
}

float UHCharaterStateComponent::GetMP() const
{
	return CurrentMP;
}

