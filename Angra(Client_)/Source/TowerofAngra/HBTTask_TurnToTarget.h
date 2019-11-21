// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "HBTTask_TurnToTarget.generated.h"

/**
 * 
 */
UCLASS()
class TOWEROFANGRA_API UHBTTask_TurnToTarget : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UHBTTask_TurnToTarget();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


	
};
