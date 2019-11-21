// Fill out your copyright notice in the Description page of Project Settings.

#include "MpPotion.h"
#include "TowerofAngraCharacter.h"

// Sets default values
AMpPotion::AMpPotion()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));
	Item = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ITEM"));
	Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EFFECT"));

	RootComponent = Trigger;
	Item->SetupAttachment(RootComponent);
	Effect->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_ITEM(TEXT("/Game/MultistoryDungeons/Meshes/Props/Potion_04.Potion_04"));
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
void AMpPotion::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMpPotion::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AMpPotion::OnCharacterOverlap);
}

// Called every frame
void AMpPotion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMpPotion::OnCharacterOverlap(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	auto Player = Cast<ATowerofAngraCharacter>(OtherActor);

	if (Player != nullptr)
	{
		Effect->Activate(true);
		Item->SetHiddenInGame(true, true);
		SetActorEnableCollision(false);
		Effect->OnSystemFinished.AddDynamic(this, &AMpPotion::OnEffectFinished);
		Player->IsFillMP = true;
		UGameplayStatics::PlaySoundAtLocation(this, Player->EatSound, GetActorLocation());
	}
}

void AMpPotion::OnEffectFinished(UParticleSystemComponent * PSystem)
{
	Destroy();
}

