// Fill out your copyright notice in the Description page of Project Settings.

#include "HSingleGamePlayerController.h"
#include "HTOAGameState.h"
#include "HResultUserWidget.h"

AHSingleGamePlayerController::AHSingleGamePlayerController()
{
	//PrimaryActorTick.bCanEverTick = true;
	static ConstructorHelpers::FClassFinder<UHResultUserWidget> UI_RESULT_C(TEXT("/Game/TowerofAngra/UI/UI_Result.UI_Result_C"));
	if (UI_RESULT_C.Succeeded())
		ResultWidgetClass = UI_RESULT_C.Class;
}

void AHSingleGamePlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AHSingleGamePlayerController::Possess(APawn * aPawn)
{
	Super::Possess(aPawn);
}

void AHSingleGamePlayerController::ChangeInputMode(bool bGameMode)
{
	if (bGameMode)
	{
		SetInputMode(GameInputMode);
		bShowMouseCursor = false;
	}
	else
	{
		SetInputMode(UIInputMode);
		bShowMouseCursor = true;
	}
}

void AHSingleGamePlayerController::ShowResultUI()
{
	auto TOAGameState = Cast<AHTOAGameState>(UGameplayStatics::GetGameState(this));

	TOAGameState->AddGameScore(CurrentScore);
	
	ResultWidget->BindGameState(TOAGameState);

	ResultWidget->AddToViewport();
	ChangeInputMode(false);
}

void AHSingleGamePlayerController::BeginPlay()
{
	Super::BeginPlay();

	ChangeInputMode(true);

	ResultWidget = CreateWidget<UHResultUserWidget>(this, ResultWidgetClass);

}

void AHSingleGamePlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHSingleGamePlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AHSingleGamePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}
