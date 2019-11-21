// Fill out your copyright notice in the Description page of Project Settings.

#include "HBigFence.h"
#include "TowerofAngraCharacter.h"

// Sets default values
AHBigFence::AHBigFence()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Fence = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FENCE"));
	OverlapVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("OVERLAPVOLUME"));

	RootComponent = Fence;
	OverlapVolume->SetupAttachment(Fence);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_FENCE(TEXT("/Game/MultistoryDungeons/Blueprints/FIRST-PERSON_V2/FP_DoorsAndBars/SM_Bars_01.SM_Bars_01"));
	if (SM_FENCE.Succeeded())
		Fence->SetStaticMesh(SM_FENCE.Object);

	OverlapVolume->SetRelativeLocation(FVector(0, 0, 200));
	OverlapVolume->InitBoxExtent(FVector(160, 350, 200));
	OverlapVolume->OnComponentBeginOverlap.AddDynamic(this, &AHBigFence::OverlapBegins);
	OverlapVolume->OnComponentEndOverlap.AddDynamic(this, &AHBigFence::OnOverlapEnd);
	IsOpenFence = false;
	IsCloseFence = true;
	fZPos = 300.f;
}

// Called when the game starts or when spawned
void AHBigFence::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHBigFence::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsOpenFence && fZPos < 300.f)
	{
		fZPos += 5.f;
		Fence->AddWorldOffset(FVector(0.f, 0.f, 5.f));
		OverlapVolume->AddWorldOffset(FVector(0.f, 0.f, -5.f));
	}
	else if (IsCloseFence && fZPos < 300.f)
	{
		fZPos += 5.f;
		Fence->AddWorldOffset(FVector(0.f, 0.f, -5.f));
		OverlapVolume->AddWorldOffset(FVector(0.f, 0.f, 5.f));
	}
}

void AHBigFence::OverlapBegins(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComponent, int32 OtherbodyIdx, bool bFromSweep, const FHitResult & SweepHit)
{
	ATowerofAngraCharacter* Character = Cast<ATowerofAngraCharacter>(OtherActor);

	if (ID == 1)
	{
		if (Character && fZPos == 300.f && IsCloseFence && CurrentButton == 8)
		{
			IsOpenFence = true;
			IsCloseFence = false;
			fZPos = 0.f;
			//CurrentButton = 0;
		}
	}
	else
	{
		if (Character && fZPos == 300.f && IsCloseFence)
		{
			IsOpenFence = true;
			IsCloseFence = false;
			fZPos = 0.f;
		}
	}
}

void AHBigFence::OnOverlapEnd(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (fZPos == 300.f && IsOpenFence && ID != 1)
	{
		IsOpenFence = false;
		IsCloseFence = true;
		fZPos = 0.f;
	}
}

