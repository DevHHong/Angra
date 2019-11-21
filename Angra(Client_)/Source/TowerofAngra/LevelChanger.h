// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TowerofAngra.h"
#include "GameFramework/Actor.h"
#include "LevelChanger.generated.h"

UCLASS()
class TOWEROFANGRA_API ALevelChanger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelChanger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OverlapBegins(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComponent, int32 OtherbodyIdx, bool bFromSweep, const FHitResult& SweepHit);
	UPROPERTY(EditAnywhere)
	FName LoadLevelName;
	UPROPERTY(EditAnywhere)
	FName OpenLevelName;

	UPROPERTY(EditAnywhere)
	class AGameModeBase* GameMode;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	class UBoxComponent* OverlapVolume;
	
};
