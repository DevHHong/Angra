// Fill out your copyright notice in the Description page of Project Settings.

#include "HMonsterHPWidget.h"
#include "HCharaterStateComponent.h"
#include "Components/ProgressBar.h"


void UHMonsterHPWidget::BindCharacterState(class UHCharaterStateComponent* NewCharacterState)
{
	CurrentCharacterState = NewCharacterState;
	CurrentCharacterState->OnHPChanged.AddUObject(this, &UHMonsterHPWidget::UpdateHPWidget);
}

void UHMonsterHPWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HPProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_HPBar")));

	UpdateHPWidget();
}

void UHMonsterHPWidget::UpdateHPWidget()
{
	if (CurrentCharacterState.IsValid())
	{
		if (HPProgressBar != nullptr)
		{
			HPProgressBar->SetPercent(CurrentCharacterState->GetHPRatio());
		}
	}
}
