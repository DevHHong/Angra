// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "HBTTask_Attack.generated.h"

/**
 * 
 */
UCLASS()
class TOWEROFANGRA_API UHBTTask_Attack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UHBTTask_Attack();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	bool IsAttacking = false;
	
	
};
