// Fill out your copyright notice in the Description page of Project Settings.

#include "HOtherPlayerHPWidget.h"
#include "HCharaterStateComponent.h"
#include "Components/ProgressBar.h"


void UHOtherPlayerHPWidget::BindCharacterState(class UHCharaterStateComponent* NewCharacterState)
{
	CurrentCharacterState = NewCharacterState;
	CurrentCharacterState->OnHPChanged.AddUObject(this, &UHOtherPlayerHPWidget::UpdateHPWidget);
}

void UHOtherPlayerHPWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HPProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_HPBar")));

	UpdateHPWidget();
}

void UHOtherPlayerHPWidget::UpdateHPWidget()
{
	if (CurrentCharacterState.IsValid())
	{
		if (HPProgressBar != nullptr)
		{
			HPProgressBar->SetPercent(CurrentCharacterState->GetHPRatio());
		}
	}
}
