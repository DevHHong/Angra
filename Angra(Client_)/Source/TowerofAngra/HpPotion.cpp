// Fill out your copyright notice in the Description page of Project Settings.

#include "HpPotion.h"
#include "TowerofAngraCharacter.h"

// Sets default values
AHpPotion::AHpPotion()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));
	Item = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ITEM"));
	Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EFFECT"));

	RootComponent = Trigger;
	Item->SetupAttachment(RootComponent);
	Effect->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_ITEM(TEXT("/Game/MultistoryDungeons/Meshes/Props/Potion_03.Potion_03"));
	if (SM_ITEM.Succeeded())
		Item->SetStaticMesh(SM_ITEM.Object);

	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_CHESTOPEN(TEXT("/Game/VFX_Toolkit_V1/ParticleSystems/356Days/Par_BuffRing_01.Par_BuffRing_01"));
	if (P_CHESTOPEN.Succeeded())
	{
		Effect->SetTemplate(P_CHESTOPEN.Object);
		Effect->bAutoActivate = false;
	}

	Trigger->SetBoxExtent(FVector(35.f, 35.f, 50.f));


	Item->SetRelativeScale3D(FVector(3.f, 3.f, 3.f));
	Item->SetRelativeLocation(FVector(0.f, 0.f, -50.f));

	Trigger->SetCollisionProfileName(TEXT("Item"));
	Item->SetCollisionProfileName(TEXT("NoCollision"));
}

// Called when the game starts or when spawned
void AHpPotion::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHpPotion::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AHpPotion::OnCharacterOverlap);
}

// Called every frame
void AHpPotion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHpPotion::OnCharacterOverlap(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	auto Player = Cast<ATowerofAngraCharacter>(OtherActor);

	if (Player != nullptr)
	{
		Effect->Activate(true);
		Item->SetHiddenInGame(true, true);
		SetActorEnableCollision(false);
		Effect->OnSystemFinished.AddDynamic(this, &AHpPotion::OnEffectFinished);
		Player->IsFillHP = true;
		UGameplayStatics::PlaySoundAtLocation(this, Player->EatSound, GetActorLocation());
	}
}

void AHpPotion::OnEffectFinished(UParticleSystemComponent * PSystem)
{
	Destroy();
}

