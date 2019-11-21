// Fill out your copyright notice in the Description page of Project Settings.

#include "HBTTask_Attack.h"
#include "HMonsterAIController.h"
#include "HMonster.h"

UHBTTask_Attack::UHBTTask_Attack()
{
	bNotifyTick = true;
	IsAttacking = false;
}

EBTNodeResult::Type UHBTTask_Attack::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto AMonster = Cast<AHMonster>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == AMonster)
		return EBTNodeResult::Failed;

	AMonster->Attack();
	IsAttacking = true;
	AMonster->OnAttackEnd.AddLambda([this]() -> void {
		IsAttacking = false;
	});

	return EBTNodeResult::InProgress;
}

void UHBTTask_Attack::TickTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if (!IsAttacking)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}



