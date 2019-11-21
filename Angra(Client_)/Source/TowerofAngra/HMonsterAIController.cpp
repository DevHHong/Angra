// Fill out your copyright notice in the Description page of Project Settings.

#include "HMonsterAIController.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

const FName AHMonsterAIController::HomePosKey(TEXT("HomePos"));
const FName AHMonsterAIController::PatrolPosKey(TEXT("PatrolPos"));
const FName AHMonsterAIController::TargetKey(TEXT("Target"));


AHMonsterAIController::AHMonsterAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(TEXT("/Game/TowerofAngra/AI/BB_Monster.BB_Monster"));
	if (BBObject.Succeeded())
		BBAsset = BBObject.Object;

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("/Game/TowerofAngra/AI/BT_Monster.BT_Monster"));
	if (BTObject.Succeeded())
		BTAsset = BTObject.Object;
}

void AHMonsterAIController::Possess(APawn * InPawn)
{
	Super::Possess(InPawn);
	/*if (UseBlackboard(BBAsset, Blackboard))
	{
		Blackboard->SetValueAsVector(HomePosKey, InPawn->GetActorLocation());
		RunBehaviorTree(BTAsset);
	}*/
		
}

void AHMonsterAIController::RunAI()
{
	if (UseBlackboard(BBAsset, Blackboard))
	{
		Blackboard->SetValueAsVector(HomePosKey, GetPawn()->GetActorLocation());
		RunBehaviorTree(BTAsset);
	}
}

void AHMonsterAIController::StopAI()
{
	auto BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (nullptr != BehaviorTreeComponent)
	{
		BehaviorTreeComponent->StopTree(EBTStopMode::Safe);
	}
}
