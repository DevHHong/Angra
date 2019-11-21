// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "Door.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "ConstructorHelpers.h"
#include "TowerofAngraCharacter.h"
#include "Engine.h"

// Sets default values
ADoor::ADoor()
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

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_FRAME(TEXT("/Game/MultistoryDungeons/Meshes/FP_Door_Bronze_01.FP_Door_Bronze_01"));
	if (SM_FRAME.Succeeded())
		DoorFrame->SetStaticMesh(SM_FRAME.Object);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_DOOR(TEXT("/Game/MultistoryDungeons/Meshes/FP_Update/Door_Bronze_01"));
	if (SM_DOOR.Succeeded())
	{
		DoorRight->SetStaticMesh(SM_DOOR.Object);
		DoorLeft->SetStaticMesh(SM_DOOR.Object);
	}

	// ป๓ลย
	DoorRight->SetRelativeLocation(FVector(-130, -30, 260));
	DoorRight->SetWorldScale3D(FVector(1, -1, 1));

	DoorLeft->SetRelativeLocation(FVector(130, -30, 260));
	DoorLeft->SetWorldRotation(FRotator(0, 181, 0));

	OverlapVolume->SetRelativeLocation(FVector(0, 0, 250));
	OverlapVolume->SetBoxExtent(FVector(200, 200, 250));
	OverlapVolume->OnComponentBeginOverlap.AddDynamic(this, &ADoor::OverlapBegins);

	CurrentAngle = DoorFrame->GetComponentRotation().Yaw - DoorRight->GetComponentRotation().Yaw;
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if ( OpenDoor && CurrentAngle < 120)
	{
		CurrentAngle += 2;
		DoorRight->AddWorldRotation(FRotator(0, -2, 0));
		DoorLeft->AddWorldRotation(FRotator(0, 2, 0));
		//GEngine->AddOnScreenDebugMessage(1, 3, FColor::Green, DoorRight->GetComponentRotation().ToString());
	}
}

void ADoor::OverlapBegins(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComponent, int32 OtherbodyIdx, bool bFromSweep, const FHitResult & SweepHit)
{
	ATowerofAngraCharacter* Character = Cast<ATowerofAngraCharacter>(OtherActor);

	if (Character)
	{
		this->OpenDoor = true;
	}
}

