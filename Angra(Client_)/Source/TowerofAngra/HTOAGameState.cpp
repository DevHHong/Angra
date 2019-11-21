// Fill out your copyright notice in the Description page of Project Settings.

#include "HTOAGameState.h"


AHTOAGameState::AHTOAGameState()
{
	TotalGameScore = 0;
	bGameCleared = false;
}

int32 AHTOAGameState::GetTotalGameScore() const
{
	return TotalGameScore;
}

void AHTOAGameState::AddGameScore(int32 Score)
{
	TotalGameScore = Score;
}

void AHTOAGameState::SetGameCleared()
{
	bGameCleared = true;
}

bool AHTOAGameState::IsGameCleared() const
{
	return bGameCleared;
}
