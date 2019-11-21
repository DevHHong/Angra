// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TowerofAngra.h"
#include "GameFramework/Actor.h"
#include "HButtonDoor.generated.h"

UCLASS()
class TOWEROFANGRA_API AHButtonDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHButtonDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = Door)
	UStaticMeshComponent* DoorLeft;
	UPROPERTY(VisibleAnywhere, Category = Door)
	UStaticMeshComponent* DoorRight;
	UPROPERTY(VisibleAnywhere, Category = Door)
	UStaticMeshComponent* DoorFrame;
	UPROPERTY(VisibleAnywhere, Category = Door)
	class UBoxComponent* OverlapVolume;
	
	UPROPERTY(EditAnywhere, Category = ID)
	int32 ID;

	UFUNCTION()
	void OverlapBegins(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComponent, int32 OtherbodyIdx, bool bFromSweep, const FHitResult & SweepHit);

private:
	UPROPERTY(EditAnywhere, Category = State, Meta = (AllowPrivateAccess = true))
	bool OpenDoor;

	UPROPERTY(EditAnywhere, Category = State, Meta = (AllowPrivateAccess = true))
	float CurrentAngle;
	
};
