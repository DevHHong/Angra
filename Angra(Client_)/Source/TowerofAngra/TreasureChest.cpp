// Fill out your copyright notice in the Description page of Project Settings.

#include "TreasureChest.h"
#include "TowerofAngraCharacter.h"

// Sets default values
ATreasureChest::ATreasureChest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	EmptyBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EMPTYBODY"));
	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BODY"));
	Cover = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cover"));
	OverlapVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("OVERLAPVOLUME"));
	Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EFFECT"));

	RootComponent = Body;
	EmptyBody->SetupAttachment(Body);
	Cover->SetupAttachment(Body);
	OverlapVolume->SetupAttachment(Body);
	Effect->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_EMPTYBODY(TEXT("/Game/MultistoryDungeons/Meshes/Props/Treasure_Chest_Base_01.Treasure_Chest_Base_01"));
	if (SM_EMPTYBODY.Succeeded())
		EmptyBody->SetStaticMesh(SM_EMPTYBODY.Object);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_BODY(TEXT("/Game/MultistoryDungeons/Meshes/Props/Treasure_Chest_Base_02.Treasure_Chest_Base_02"));
	if (SM_BODY.Succeeded())
		Body->SetStaticMesh(SM_BODY.Object);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_COVER(TEXT("/Game/MultistoryDungeons/Meshes/Props/Treasure_Chest_lid_01.Treasure_Chest_lid_01"));
	if (SM_COVER.Succeeded())
		Cover->SetStaticMesh(SM_COVER.Object);

	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_CHESTOPEN(TEXT("/Game/InfinityBladeGrassLands/Effects/FX_Treasure/Chest/P_TreasureChest_Open_Mesh.P_TreasureChest_Open_Mesh"));
	if (P_CHESTOPEN.Succeeded())
	{
		Effect->SetTemplate(P_CHESTOPEN.Object);
		Effect->bAutoActivate = false;
	}

	Cover->SetRelativeLocation(FVector(0, -34, 45));

	OverlapVolume->SetRelativeLocation(FVector(0, 120, 100));
	OverlapVolume->SetBoxExtent(FVector(100, 150, 100));
	OverlapVolume->OnComponentBeginOverlap.AddDynamic(this, &ATreasureChest::OverlapBegins);
	
	Effect->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	IsOpen = false;
	IsHave = false;
	IsEmpty = false;
}

// Called when the game starts or when spawned
void ATreasureChest::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATreasureChest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsOpen && Cover->RelativeRotation.Roll > -130.f)
	{
		Cover->AddRelativeRotation(FRotator(0, 0, -1));
		//GEngine->AddOnScreenDebugMessage(1, 5, FColor::Blue, FString::Printf(TEXT("Chest head angle : %f"), head->RelativeRotation.Roll));
	}
	else if (IsOpen && Cover->RelativeRotation.Roll <= -130.f)
		IsHave = true;
}

void ATreasureChest::OverlapBegins(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComponent, int32 OtherbodyIdx, bool bFromSweep, const FHitResult & SweepHit)
{
	ATowerofAngraCharacter * Character = Cast<ATowerofAngraCharacter>(OtherActor);

	if (Character&& !IsOpen)
	{
		IsOpen = true;
		OverlapVolume->SetRelativeLocation(FVector(0, 40, 100));
		OverlapVolume->SetBoxExtent(FVector(100, 70, 100));
	}
	
	if (IsHave && !IsEmpty)
	{
		CurrentScore += 20;
		Effect->Activate(true);
		Body->SetStaticMesh(EmptyBody->GetStaticMesh());
		IsEmpty = true;
		//Effect->OnSystemFinished.AddDynamic(this, &ATreasureChest::OnEffectFinished);
	}
		
}

void ATreasureChest::OnOverlapEnd(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
}

void ATreasureChest::OnEffectFinished(UParticleSystemComponent * PSystem)
{
	Destroy();
}

