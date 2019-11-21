// Fill out your copyright notice in the Description page of Project Settings.

#include "HBTTask_TurnToTarget.h"
#include "HMonsterAIController.h"
#include "HMonster.h"
#include "TowerofAngraCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"



UHBTTask_TurnToTarget::UHBTTask_TurnToTarget()
{
	NodeName = TEXT("Turn");
}

EBTNodeResult::Type UHBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto AMonster = Cast<AHMonster>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == AMonster)
		return EBTNodeResult::Failed;


	auto Target = Cast<ATowerofAngraCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AHMonsterAIController::TargetKey));

	if (nullptr == Target)
		return EBTNodeResult::Failed;

	FVector LookVector = Target->GetActorLocation() - AMonster->GetActorLocation();
	LookVector.Z = 0.f;
	FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
	AMonster->SetActorRotation(FMath::RInterpTo(AMonster->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 2.f));

	return EBTNodeResult::Succeeded;
}



