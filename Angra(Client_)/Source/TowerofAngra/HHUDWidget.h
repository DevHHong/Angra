// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TowerofAngra.h"
#include "Blueprint/UserWidget.h"
#include "HHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class TOWEROFANGRA_API UHHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void BindCharacterState(class UHCharaterStateComponent* NewCharacterState);

protected:
	virtual void NativeConstruct() override;

	void UpdateCharacterHPState();
	void UpdateCharacterMPState();
private:
	TWeakObjectPtr<class UHCharaterStateComponent> CurrentCharacterState;
	
	UPROPERTY()
	class UProgressBar* HPBar;

	UPROPERTY()
	class UProgressBar* MPBar;
};
