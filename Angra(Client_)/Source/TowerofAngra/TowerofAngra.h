// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EngineMinimal.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine.h"

UENUM(BlueprintType)
enum class EPlayerType : uint8
{
	KAYA,
	AXE,
	WARRIOR,
	NONE
};

UENUM(BlueprintType)
enum class EGameMode : uint8
{
	SINGLE_GAME,
	MULTI_GAME,
	NONE
};

static EPlayerType PlayerType{ EPlayerType::NONE };
static EGameMode CurrentGameMode{ EGameMode::NONE };
static FString IPaddress{};
static int CurrentStageLevel = 0;		// 현재 맵 스테이지 레벨 
static int CurrentScore = 0;			// 현재 점수
static int CurrentButton = 0;

DECLARE_LOG_CATEGORY_EXTERN(TowerofAngra, Log, All);
#define TOALOG_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define TOALOG_S(Verbosity) UE_LOG(TowerofAngra, Verbosity, TEXT("%s"), *TOALOG_CALLINFO)
#define TOALOG(Verbosity, Format, ...) UE_LOG(TowerofAngra, Verbosity, TEXT("%s %s"),*TOALOG_CALLINFO, *FString::Printf(Format,##__VA_ARGS__))