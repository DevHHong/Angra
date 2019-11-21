// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "TowerofAngra.h"
#include "GameFramework/GameModeBase.h"
#include "TowerofAngraGameMode.generated.h"

UCLASS()
class TOWEROFANGRA_API ATowerofAngraGameMode : public AGameModeBase
{
	GENERATED_BODY()

	ATowerofAngraGameMode();

public:
	virtual void PostInitializeComponents() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	void AddScore();
	int32 GetScore() const;

private:
	UPROPERTY()
	class AHTOAGameState* TOAGameState;

	UPROPERTY()
	int32 ScoreToClear;
};



