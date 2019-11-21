// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "TowerofAngraGameMode.h"
#include "HKayaCharacter.h"
#include "HAxeCharacter.h"
#include "HWarriorCharacter.h"
#include "CharacterPlayerController.h"
#include "HSingleGamePlayerController.h"
#include "HTOAGameState.h"

ATowerofAngraGameMode::ATowerofAngraGameMode()
{
	// 초기화
	CurrentStageLevel = 0;
	CurrentScore = 0;
	/*CurrentGameMode = EGameMode::MULTI_GAME;
	PlayerType = EPlayerType::WARRIOR;*/

	// 연습용
	if (PlayerType == EPlayerType::NONE && CurrentGameMode == EGameMode::NONE)
	{
		DefaultPawnClass = AHWarriorCharacter::StaticClass();
		PlayerControllerClass = AHSingleGamePlayerController::StaticClass();
	}

	// set set default character class by c++
	if (PlayerType  == EPlayerType::WARRIOR)
	{
		DefaultPawnClass = AHWarriorCharacter::StaticClass();
	}
	else if(PlayerType == EPlayerType::AXE)
	{
		DefaultPawnClass = AHAxeCharacter::StaticClass();
	}
	
	// set default character controller class by c++
	if (CurrentGameMode == EGameMode::MULTI_GAME)
	{
		PlayerControllerClass = ACharacterPlayerController::StaticClass();
	}
	else if (CurrentGameMode == EGameMode::SINGLE_GAME)
	{
		PlayerControllerClass = AHSingleGamePlayerController::StaticClass();
	}

	GameStateClass = AHTOAGameState::StaticClass();
	TOALOG(Warning, TEXT("CurrentGameMode : %d"), int(CurrentGameMode));
}

void ATowerofAngraGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	TOAGameState = Cast<AHTOAGameState>(GameState);
}

void ATowerofAngraGameMode::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void ATowerofAngraGameMode::AddScore()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		const auto PlayerController1 = Cast<AHSingleGamePlayerController>(It->Get());
		if (nullptr != PlayerController1)
		{
			PlayerController1->ShowResultUI();
		}

		const auto PlayerController2 = Cast<ACharacterPlayerController>(It->Get());
		if (nullptr != PlayerController2)
		{
			PlayerController2->ShowResultUI();
		}
	}
}

int32 ATowerofAngraGameMode::GetScore() const
{
	return TOAGameState->GetTotalGameScore();
}
