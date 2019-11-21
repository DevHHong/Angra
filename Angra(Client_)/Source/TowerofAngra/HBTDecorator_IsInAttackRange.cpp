// Fill out your copyright notice in the Description page of Project Settings.

#include "HBTDecorator_IsInAttackRange.h"
#include "HMonsterAIController.h"
#include "HMonster.h"
#include "TowerofAngraCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"



UHBTDecorator_IsInAttackRange::UHBTDecorator_IsInAttackRange()
{
	NodeName = TEXT("CanAttack");
}

bool UHBTDecorator_IsInAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	auto ControllingPawn = Cast<AHMonster>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == ControllingPawn)
		return false;

	auto Target = Cast<ATowerofAngraCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AHMonsterAIController::TargetKey));

	if (nullptr == Target)
		return false;

	//bResult = (Target->GetDistanceTo(ControllingPawn) <= ControllingPawn->GetAttackRange());				// 3D 거리
	bResult = (Target->GetHorizontalDistanceTo(ControllingPawn) <= ControllingPawn->GetAttackRange());		// 2D 거리
	
	return bResult;
}




