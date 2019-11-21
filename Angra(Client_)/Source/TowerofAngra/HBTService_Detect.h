// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "HBTService_Detect.generated.h"

/**
 * 
 */
UCLASS()
class TOWEROFANGRA_API UHBTService_Detect : public UBTService
{
	GENERATED_BODY()
	
public:
	UHBTService_Detect();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	
	
};
