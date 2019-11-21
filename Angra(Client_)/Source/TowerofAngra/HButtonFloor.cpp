// Fill out your copyright notice in the Description page of Project Settings.

#include "HButtonFloor.h"
#include "TowerofAngraCharacter.h"
#include "Engine.h"

// Sets default values
AHButtonFloor::AHButtonFloor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Button = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BUTTON"));
	OverlapVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("OVERLAPVOLUME"));

	RootComponent = Button;
	OverlapVolume->SetupAttachment(Button);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_BUTTON(TEXT("/Game/MultistoryDungeons/Meshes/Pedestal_01.Pedestal_01"));
	if (SM_BUTTON.Succeeded())
		Button->SetStaticMesh(SM_BUTTON.Object);

	OverlapVolume->SetRelativeLocation(FVector(0, 0, 200));
	OverlapVolume->InitBoxExtent(FVector(70, 70, 10));
	OverlapVolume->OnComponentBeginOverlap.AddDynamic(this, &AHButtonFloor::OverlapBegins);

	IsOnButton = false;
	fZPos = 0.f;
}

// Called when the game starts or when spawned
void AHButtonFloor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHButtonFloor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsOnButton && fZPos <= 160)
	{
		fZPos += 2.f;
		Button->AddWorldOffset(FVector(0.f, 0.f, -2.f));
	}

}

void AHButtonFloor::OverlapBegins(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComponent, int32 OtherbodyIdx, bool bFromSweep, const FHitResult & SweepHit)
{
	ATowerofAngraCharacter* Character = Cast<ATowerofAngraCharacter>(OtherActor);

	if (!IsOnButton)
	{
		++CurrentButton;
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("CurrentButton : %d"), CurrentButton));
	}

	if (Character)
		IsOnButton = true;
}

