// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TowerofAngra.h"
#include "BehaviorTree/BTDecorator.h"
#include "HBTDecorator_IsInAttackRange.generated.h"

/**
 * 
 */
UCLASS()
class TOWEROFANGRA_API UHBTDecorator_IsInAttackRange : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UHBTDecorator_IsInAttackRange();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

};
