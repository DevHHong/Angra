// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HMonsterHPWidget.generated.h"

/**
 * 
 */
UCLASS()
class TOWEROFANGRA_API UHMonsterHPWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void BindCharacterState(class UHCharaterStateComponent* NewCharacterState);

protected:
	virtual void NativeConstruct() override;
	void UpdateHPWidget();

private:
	TWeakObjectPtr<class UHCharaterStateComponent> CurrentCharacterState;

	UPROPERTY()
	class UProgressBar* HPProgressBar;
	
};
