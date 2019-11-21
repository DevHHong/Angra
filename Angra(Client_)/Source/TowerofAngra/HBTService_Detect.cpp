// Fill out your copyright notice in the Description page of Project Settings.

#include "HBTService_Detect.h"
#include "HMonsterAIController.h"
#include "HMonster.h"
#include "TowerofAngraCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"


UHBTService_Detect::UHBTService_Detect()
{
	NodeName = TEXT("Detect");
	Interval = 1.f;
}

void UHBTService_Detect::TickNode(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn) return;
	
	AHMonster* AMonster = Cast<AHMonster>(ControllingPawn);

	UWorld* World = ControllingPawn->GetWorld();
	FVector Center = ControllingPawn->GetActorLocation();

	float DetectRadius = AMonster->GetDetectRadius();

	if (nullptr == World) return;
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, ControllingPawn);
	bool bResult = World->OverlapMultiByChannel(
		OverlapResults, Center,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(DetectRadius),
		CollisionQueryParam
	);


	// Player Target 했을시.
	if (bResult)
	{
		for (auto OverlapResult : OverlapResults)
		{
			ATowerofAngraCharacter* APlayer = Cast< ATowerofAngraCharacter>(OverlapResult.GetActor());

			if (APlayer/*&& APlayer->GetController()->IsPlayerController()*/)
			{
				AMonster->SetHpShow(true);
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(AHMonsterAIController::TargetKey, APlayer);

				//DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Green, false, 0.2f);

				DrawDebugPoint(World, APlayer->GetActorLocation(), 10.f, FColor::Blue, false, 0.2f);

				//DrawDebugLine(World, AMonster->GetActorLocation(), APlayer->GetActorLocation(), FColor::Blue, false, 0.2f);

				return;
			}
		}
	}

	// 범위를 벗어나면 다시 타겟키를 null로 만든다.
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(AHMonsterAIController::TargetKey, nullptr);
	AMonster->SetHpShow(false);
	//DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Red, false, 0.2f);
}
