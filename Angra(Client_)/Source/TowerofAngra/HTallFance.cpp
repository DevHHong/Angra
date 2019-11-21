// Fill out your copyright notice in the Description page of Project Settings.

#include "Engine.h"
#include "HTallFance.h"
#include "TowerofAngraCharacter.h"
#include "CharacterPlayerController.h"
// Sets default values
AHTallFance::AHTallFance()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Fence = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FENCE"));
	OverlapVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("OVERLAPVOLUME"));

	RootComponent = Fence;
	OverlapVolume->SetupAttachment(Fence);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_FENCE(TEXT("/Game/MultistoryDungeons/Blueprints/FIRST-PERSON_V2/FP_DoorsAndBars/SM_Arch_Tall_Bars_01.SM_Arch_Tall_Bars_01"));
	if (SM_FENCE.Succeeded())
		Fence->SetStaticMesh(SM_FENCE.Object);

	OverlapVolume->SetRelativeLocation(FVector(0, 0, 200));
	OverlapVolume->InitBoxExtent(FVector(160, 250, 200));
	OverlapVolume->OnComponentBeginOverlap.AddDynamic(this, &AHTallFance::OverlapBegins);
	IsOpenFence = false;
	fZPos = 500.f;
}

// Called when the game starts or when spawned
void AHTallFance::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AHTallFance::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsOpenFence && fZPos < 500.f)
	{
		fZPos += 5.f;
		Fence->AddWorldOffset(FVector(0.f, 0.f, 5.f));
		OverlapVolume->AddWorldOffset(FVector(0.f, 0.f, -5.f));
	}
}

void AHTallFance::OverlapBegins(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComponent, int32 OtherbodyIdx, bool bFromSweep, const FHitResult & SweepHit)
{
	ATowerofAngraCharacter* Character = Cast<ATowerofAngraCharacter>(OtherActor);

	if (CurrentGameMode == EGameMode::MULTI_GAME)
	{
		//ACharacterPlayerController* PlayerController = Cast<ACharacterPlayerController>(GetWorld()->GetFirstPlayerController());
		//int MonsterNum = PlayerController->TOAMonsterset->monsters.size();

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("MonsterNum  %d "), MonsterNum));
		if ((Character && !IsOpenFence) /*&& (MonsterNum<8)*/)
		{
			IsOpenFence = true;
			fZPos = 0.f;
		}
	}
	else
	{
		if (Character && !IsOpenFence)
		{
			IsOpenFence = true;
			fZPos = 0.f;
		}
	}
}

