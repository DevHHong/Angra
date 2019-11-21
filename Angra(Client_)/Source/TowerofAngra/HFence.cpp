// Fill out your copyright notice in the Description page of Project Settings.

#include "HFence.h"
#include "TowerofAngraCharacter.h"
#include "Engine.h"

// Sets default values
AHFence::AHFence()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Fence = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FENCE"));
	OverlapVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("OVERLAPVOLUME"));

	RootComponent = Fence;
	OverlapVolume->SetupAttachment(Fence);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_FENCE(TEXT("/Game/MultistoryDungeons/Blueprints/FIRST-PERSON_V2/FP_DoorsAndBars/SM_Bars_04.SM_Bars_04"));
	if (SM_FENCE.Succeeded())
		Fence->SetStaticMesh(SM_FENCE.Object);

	OverlapVolume->SetRelativeLocation(FVector(0, 50, 200));
	OverlapVolume->InitBoxExtent(FVector(100, 200, 200));
	OverlapVolume->OnComponentBeginOverlap.AddDynamic(this, &AHFence::OverlapBegins);
	OverlapVolume->OnComponentEndOverlap.AddDynamic(this, &AHFence::OnOverlapEnd);
	IsOpenFence = false;
	IsCloseFence = true;
	fZPos = 250.f;
}

// Called when the game starts or when spawned
void AHFence::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHFence::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsOpenFence && fZPos < 250.f)
	{
		fZPos += 5.f;
		Fence->AddWorldOffset(FVector(0.f, 0.f, 5.f));
		OverlapVolume->AddWorldOffset(FVector(0.f, 0.f, -5.f));
	}
	else if (IsCloseFence && fZPos < 250.f)
	{
		fZPos += 5.f;
		Fence->AddWorldOffset(FVector(0.f, 0.f, -5.f));
		OverlapVolume->AddWorldOffset(FVector(0.f, 0.f, 5.f));
	}
}

void AHFence::OverlapBegins(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComponent, int32 OtherbodyIdx, bool bFromSweep, const FHitResult & SweepHit)
{
	ATowerofAngraCharacter* Character = Cast<ATowerofAngraCharacter>(OtherActor);

	if (Character && fZPos == 250 && IsCloseFence)
	{
		IsOpenFence = true;
		IsCloseFence = false;
		fZPos = 0.f;
	}
}

void AHFence::OnOverlapEnd(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (fZPos == 250 && IsOpenFence)
	{
		IsOpenFence = false;
		IsCloseFence = true;
		fZPos = 0.f;
	}
}

