// Fill out your copyright notice in the Description page of Project Settings.

#include "HButtonDoor.h"
#include "Engine.h"
#include "TowerofAngraCharacter.h"

// Sets default values
AHButtonDoor::AHButtonDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DOORFRAME"));
	DoorRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RIGHT"));
	DoorLeft = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LEFT"));
	OverlapVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("OVERLAPVOLUME"));

	RootComponent = DoorFrame;
	DoorRight->SetupAttachment(DoorFrame);
	DoorLeft->SetupAttachment(DoorFrame);
	OverlapVolume->SetupAttachment(DoorFrame);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_FRAME(TEXT("/Game/MultistoryDungeons/Meshes/FP_Door_01.FP_Door_01"));
	if (SM_FRAME.Succeeded())
		DoorFrame->SetStaticMesh(SM_FRAME.Object);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_DOOR(TEXT("/Game/MultistoryDungeons/Meshes/Props/Door_01.Door_01"));
	if (SM_DOOR.Succeeded())
	{
		DoorRight->SetStaticMesh(SM_DOOR.Object);
		DoorLeft->SetStaticMesh(SM_DOOR.Object);
	}

	// ป๓ลย
	DoorRight->SetRelativeLocation(FVector(-130, -30, 0));
	DoorRight->SetWorldScale3D(FVector(1, -1, 1));

	DoorLeft->SetRelativeLocation(FVector(130, -30, 0));
	DoorLeft->SetWorldRotation(FRotator(0, 181, 0));

	OverlapVolume->SetRelativeLocation(FVector(0, 0, 250));
	OverlapVolume->SetBoxExtent(FVector(200, 200, 250));
	OverlapVolume->OnComponentBeginOverlap.AddDynamic(this, &AHButtonDoor::OverlapBegins);

	CurrentAngle = DoorFrame->GetComponentRotation().Yaw - DoorRight->GetComponentRotation().Yaw;
}

// Called when the game starts or when spawned
void AHButtonDoor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHButtonDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ID == 1 && CurrentButton == 4)
	{
		OpenDoor = true;
	}
	
	if (OpenDoor && CurrentAngle < 140)
	{
		CurrentAngle += 2;
		DoorRight->AddWorldRotation(FRotator(0, -2, 0));
		DoorLeft->AddWorldRotation(FRotator(0, 2, 0));
	}
}

void AHButtonDoor::OverlapBegins(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComponent, int32 OtherbodyIdx, bool bFromSweep, const FHitResult & SweepHit)
{
	/*ATowerofAngraCharacter* Character = Cast<ATowerofAngraCharacter>(OtherActor);
	if(ID==1 && CurrentButton==1)
		OpenDoor = true;
	else if (ID == 2 && CurrentButton == 2)
		OpenDoor = true;
	
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("CurrentButton : %d"), CurrentButton));*/
}

