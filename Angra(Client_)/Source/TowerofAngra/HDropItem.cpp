// Fill out your copyright notice in the Description page of Project Settings.

#include "HDropItem.h"
#include "TowerofAngraCharacter.h"

// Sets default values
AHDropItem::AHDropItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));
	Item = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ITEM"));
	Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EFFECT"));
	
	RootComponent = Trigger;
	Item->SetupAttachment(RootComponent);
	Effect->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_ITEM(TEXT("/Game/MultistoryDungeons/Meshes/Props/Potion_Blue_01.Potion_Blue_01"));
	if (SM_ITEM.Succeeded())
		Item->SetStaticMesh(SM_ITEM.Object);

	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_CHESTOPEN(TEXT("/Game/VFX_Toolkit_V1/ParticleSystems/356Days/Par_BuffRing_01.Par_BuffRing_01"));
	if (P_CHESTOPEN.Succeeded())
	{
		Effect->SetTemplate(P_CHESTOPEN.Object);
		Effect->bAutoActivate = false;
	}

	Trigger->SetBoxExtent(FVector(20.f, 20.f, 30.f));

	
	Item->SetRelativeScale3D(FVector(2.f, 2.f, 2.f));
	Item->SetRelativeLocation(FVector(0.f, 0.f, -30.f));
	
	Trigger->SetCollisionProfileName(TEXT("Item"));
	Item->SetCollisionProfileName(TEXT("NoCollision"));
}

// Called when the game starts or when spawned
void AHDropItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHDropItem::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AHDropItem::OnCharacterOverlap);
}

// Called every frame
void AHDropItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHDropItem::OnCharacterOverlap(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	auto Player = Cast<ATowerofAngraCharacter>(OtherActor);
	
	if (Player != nullptr)
	{
		Effect->Activate(true);
		Item->SetHiddenInGame(true, true);
		SetActorEnableCollision(false);
		Effect->OnSystemFinished.AddDynamic(this, &AHDropItem::OnEffectFinished);
		Player->IsFillHP = true;
		UGameplayStatics::PlaySoundAtLocation(this, Player->EatSound, GetActorLocation());
	}
}

void AHDropItem::OnEffectFinished(UParticleSystemComponent * PSystem)
{
	Destroy();
}

