// Fill out your copyright notice in the Description page of Project Settings.

#include "HSingleSelectUserWidget.h"
#include "Components/Button.h"

void UHSingleSelectUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	WarriorButton = Cast<UButton>(GetWidgetFromName(TEXT("btnWarrior")));

	AxeButton = Cast<UButton>(GetWidgetFromName(TEXT("btnAxe")));

	BackButton = Cast<UButton>(GetWidgetFromName(TEXT("btnBack")));

	WarriorButton->OnClicked.AddDynamic(this, &UHSingleSelectUserWidget::OnWorriorButtonClicked);

	AxeButton->OnClicked.AddDynamic(this, &UHSingleSelectUserWidget::OnAxeButtonClicked);

	BackButton->OnClicked.AddDynamic(this, &UHSingleSelectUserWidget::OnBackButtonClicked);

	CurrentGameMode = EGameMode::SINGLE_GAME;
}

void UHSingleSelectUserWidget::OnWorriorButtonClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("TOA_Stage"));

	PlayerType = EPlayerType::WARRIOR;
}

void UHSingleSelectUserWidget::OnAxeButtonClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("TOA_Stage"));

	PlayerType = EPlayerType::AXE;
}

void UHSingleSelectUserWidget::OnBackButtonClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Title"));
}




