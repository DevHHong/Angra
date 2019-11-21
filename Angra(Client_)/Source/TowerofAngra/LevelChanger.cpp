// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "TowerofAngraGameMode.h"
#include "LevelChanger.h"
#include "TowerofAngraCharacter.h"
#include "Engine.h"
// Sets default values
ALevelChanger::ALevelChanger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	OverlapVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapVolume"));
	OverlapVolume->OnComponentBeginOverlap.AddUniqueDynamic(this, &ALevelChanger::OverlapBegins);
	RootComponent = OverlapVolume;
}

// Called when the game starts or when spawned
void ALevelChanger::BeginPlay()
{
	Super::BeginPlay();
	FVector SetLocation;
	SetLocation = FVector(0, 0, 150);

	GameMode = GetWorld()->GetAuthGameMode();
}

// Called every frame
void ALevelChanger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALevelChanger::OverlapBegins(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComponent, int32 OtherbodyIdx, bool bFromSweep, const FHitResult & SweepHit)
{
	//ACharacter* MyChar = UGameplayStatics::GetPlayerCharacter(this, 0);

	ATowerofAngraCharacter * Player = Cast<ATowerofAngraCharacter>(OtherActor);

	if (Player && LoadLevelName != "")
	{
		FLatentActionInfo ActionInfo;

		UGameplayStatics::LoadStreamLevel(this, LoadLevelName, true, true, ActionInfo);

		if (LoadLevelName == "TOA_Stage2" || LoadLevelName == "MTOA_Stage2")
		{
			CurrentStageLevel = 2;
			auto GameMode = Cast<ATowerofAngraGameMode>(GetWorld()->GetAuthGameMode());

			GameMode->AddScore();
		}
		else if (LoadLevelName == "TOA_Stage1" || LoadLevelName == "MTOA_Stage1")
		{
			CurrentStageLevel = 1;
		}
		TOALOG(Warning, TEXT("LoadLevelName : %s"), *LoadLevelName.ToString());
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("StageLevel  %d "), CurrentStageLevel));
	}

	/*if (OtherActor == MyChar && OpenLevelName != "")
	{
		GameMode->SetPlayerDefaults(MyChar);
		UGameplayStatics::OpenLevel(this, OpenLevelName);
	}*/
}

