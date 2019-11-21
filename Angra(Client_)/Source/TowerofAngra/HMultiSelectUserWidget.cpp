// Fill out your copyright notice in the Description page of Project Settings.

#include "Engine.h"
#include "HMultiSelectUserWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
void UHMultiSelectUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	WarriorButton = Cast<UButton>(GetWidgetFromName(TEXT("btnWarrior")));
	
	AxeButton = Cast<UButton>(GetWidgetFromName(TEXT("btnAxe")));

	BackButton = Cast<UButton>(GetWidgetFromName(TEXT("btnBack")));

	InputButton = Cast<UButton>(GetWidgetFromName(TEXT("btnInput")));

	TextBox = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("IPaddrTextBox")));
	
	WarriorButton->OnClicked.AddDynamic(this, &UHMultiSelectUserWidget::OnWorriorButtonClicked);

	AxeButton->OnClicked.AddDynamic(this, &UHMultiSelectUserWidget::OnAxeButtonClicked);

	BackButton->OnClicked.AddDynamic(this, &UHMultiSelectUserWidget::OnBackButtonClicked);

	InputButton->OnClicked.AddDynamic(this, &UHMultiSelectUserWidget::OnInputButtonClicked);

	CurrentGameMode = EGameMode::MULTI_GAME;
}

void UHMultiSelectUserWidget::OnWorriorButtonClicked()
{
	if (IPaddress.Len() <= 0 || IPaddress.Len() > 15) return;

	PlayerType = EPlayerType::WARRIOR;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%d"),int(PlayerType)));
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("MTOA_Stage"));
}

void UHMultiSelectUserWidget::OnAxeButtonClicked()
{
	if (IPaddress.Len() <= 0 || IPaddress.Len() > 15) return;

	PlayerType = EPlayerType::AXE;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%d"), int(PlayerType)));
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("MTOA_Stage"));
}

void UHMultiSelectUserWidget::OnBackButtonClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Title"));
}

void UHMultiSelectUserWidget::OnInputButtonClicked()
{
	IPaddress = TextBox->GetText().ToString();
}
