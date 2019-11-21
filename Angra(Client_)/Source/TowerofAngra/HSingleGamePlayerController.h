// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TowerofAngra.h"
#include "GameFramework/PlayerController.h"
#include "HSingleGamePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TOWEROFANGRA_API AHSingleGamePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AHSingleGamePlayerController();

	virtual void PostInitializeComponents() override;
	virtual void Possess(APawn* aPawn) override;

	void ChangeInputMode(bool bGameMode = true);
	void ShowResultUI();
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void SetupInputComponent() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UHResultUserWidget> ResultWidgetClass;

private:
	UPROPERTY()
	class AHTOAGameState* TOAPlayerState;

	UPROPERTY()
	class UHResultUserWidget* ResultWidget;

	FInputModeGameOnly GameInputMode;
	FInputModeUIOnly UIInputMode;
};
