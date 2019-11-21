// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TowerofAngra.h"
#include "Blueprint/UserWidget.h"
#include "HSingleSelectUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class TOWEROFANGRA_API UHSingleSelectUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY()
	class UButton* WarriorButton;

	UPROPERTY()
	class UButton* AxeButton;

	UPROPERTY()
	class UButton* BackButton;

private:
	UFUNCTION()
	void OnWorriorButtonClicked();

	UFUNCTION()
	void OnAxeButtonClicked();

	UFUNCTION()
	void OnBackButtonClicked();
};
