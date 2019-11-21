// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TowerofAngra.h"
#include "GameFramework/Actor.h"
#include "HButtonFloor.generated.h"

UCLASS()
class TOWEROFANGRA_API AHButtonFloor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHButtonFloor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = Button)
	UStaticMeshComponent* Button;
	UPROPERTY(VisibleAnywhere, Category = Button)
	class UBoxComponent* OverlapVolume;

	UFUNCTION()
	void OverlapBegins(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComponent, int32 OtherbodyIdx, bool bFromSweep, const FHitResult & SweepHit);

private:
	UPROPERTY(EditAnywhere, Category = State, Meta = (AllowPrivateAccess = true))
	bool IsOnButton;

	UPROPERTY(EditAnywhere, Category = State, Meta = (AllowPrivateAccess = true))
	float fZPos;
};
