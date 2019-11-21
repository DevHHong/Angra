// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TowerofAngra.h"
#include "GameFramework/Actor.h"
#include "TreasureChest.generated.h"

UCLASS()
class TOWEROFANGRA_API ATreasureChest : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATreasureChest();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = TreasureChest)
	UStaticMeshComponent* EmptyBody;
	UPROPERTY(EditAnywhere, Category = TreasureChest)
	UStaticMeshComponent* Body;
	UPROPERTY(EditAnywhere, Category = TreasureChest)
	UStaticMeshComponent* Cover;
	UPROPERTY(EditAnywhere, Category = TreasureChest)
	class UBoxComponent* OverlapVolume;

	UPROPERTY(VisibleAnywhere, CAtegory = Effect)
	UParticleSystemComponent* Effect;

	UFUNCTION()
	void OverlapBegins(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor,
			UPrimitiveComponent * OtherComponent, int32 OtherbodyIdx, bool bFromSweep, const FHitResult & SweepHit);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnEffectFinished(class UParticleSystemComponent* PSystem);

	bool IsOpen;
	bool IsHave;
	bool IsEmpty;
};
