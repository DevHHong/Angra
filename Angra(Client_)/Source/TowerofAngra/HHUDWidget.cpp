// Fill out your copyright notice in the Description page of Project Settings.

#include "HHUDWidget.h"
#include "HCharaterStateComponent.h"
#include "Components/ProgressBar.h"


void UHHUDWidget::BindCharacterState(class UHCharaterStateComponent* NewCharacterState)
{
	CurrentCharacterState = NewCharacterState;
	CurrentCharacterState->OnHPChanged.AddUObject(this, &UHHUDWidget::UpdateCharacterHPState);
	CurrentCharacterState->OnMPChanged.AddUObject(this, &UHHUDWidget::UpdateCharacterMPState);
}

void UHHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	HPBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("pbHP")));
	
	MPBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("pbMP")));
}

void UHHUDWidget::UpdateCharacterHPState()
{
	HPBar->SetPercent(CurrentCharacterState->GetHPRatio());
}

void UHHUDWidget::UpdateCharacterMPState()
{
	MPBar->SetPercent(CurrentCharacterState->GetMPRatio());
}
