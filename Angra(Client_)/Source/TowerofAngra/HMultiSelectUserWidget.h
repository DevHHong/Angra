// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TowerofAngra.h"
#include "Blueprint/UserWidget.h"
#include "HMultiSelectUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class TOWEROFANGRA_API UHMultiSelectUserWidget : public UUserWidget
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

	UPROPERTY()
	class UButton* InputButton;

	UPROPERTY()
	class UEditableTextBox* TextBox;

private:
	UFUNCTION()
	void OnWorriorButtonClicked();

	UFUNCTION()
	void OnAxeButtonClicked();
	
	UFUNCTION()
	void OnBackButtonClicked();

	UFUNCTION()
	void OnInputButtonClicked();
};
