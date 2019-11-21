// Fill out your copyright notice in the Description page of Project Settings.

#include "HAxeWeaponItem.h"
#include "TowerofAngraCharacter.h"
#include "HAxeCharacter.h"

// Sets default values
AHAxeWeaponItem::AHAxeWeaponItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));
	Item = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ITEM"));
	Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EFFECT"));
	Movement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("MOVEMENT"));

	RootComponent = Trigger;
	Item->SetupAttachment(RootComponent);
	Effect->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_ITEM(TEXT("/Game/InfinityBladeWeapons/Weapons/Blade/Axes/Blade_TAxe/SM_Blade_TAxe.SM_Blade_TAxe"));
	if (SM_ITEM.Succeeded())
		Item->SetStaticMesh(SM_ITEM.Object);

	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_CHESTOPEN(TEXT("/Game/VFX_Toolkit_V1/ParticleSystems/356Days/Par_BuffRing_01.Par_BuffRing_01"));
	if (P_CHESTOPEN.Succeeded())
	{
		Effect->SetTemplate(P_CHESTOPEN.Object);
		Effect->bAutoActivate = false;
	}

	Trigger->SetBoxExtent(FVector(20.f, 35.f, 60.f));

	Item->SetRelativeLocationAndRotation(FVector(0.f, -20.f, -30.f), FRotator(0.f, 0.f, 35.f));

	Trigger->SetCollisionProfileName(TEXT("Item"));
	Item->SetCollisionProfileName(TEXT("NoCollision"));

	RotateSpeed = 50.f;
}

// Called when the game starts or when spawned
void AHAxeWeaponItem::BeginPlay()
{
	Super::BeginPlay();
}

void AHAxeWeaponItem::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AHAxeWeaponItem::OnCharacterOverlap);
}

// Called every frame
void AHAxeWeaponItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddActorLocalRotation(FRotator(0.f, RotateSpeed * DeltaTime, 0.f));
}

void AHAxeWeaponItem::OnCharacterOverlap(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	auto Player = Cast<AHAxeCharacter>(OtherActor);

	if (Player != nullptr)
	{
		Effect->Activate(true);
		Item->SetHiddenInGame(true, true);
		SetActorEnableCollision(false);
		Effect->OnSystemFinished.AddDynamic(this, &AHAxeWeaponItem::OnEffectFinished);
		Player->WeaponChange();
	}
}

void AHAxeWeaponItem::OnEffectFinished(UParticleSystemComponent * PSystem)
{
	Destroy();
}

