// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HTallFance.generated.h"

UCLASS()
class TOWEROFANGRA_API AHTallFance : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHTallFance();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = Fence)
	UStaticMeshComponent* Fence;
	UPROPERTY(VisibleAnywhere, Category = Fence)
	class UBoxComponent* OverlapVolume;

	UFUNCTION()
	void OverlapBegins(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComponent, int32 OtherbodyIdx, bool bFromSweep, const FHitResult & SweepHit);

private:
	UPROPERTY(EditAnywhere, Category = State, Meta = (AllowPrivateAccess = true))
	bool IsOpenFence;

	UPROPERTY(EditAnywhere, Category = State, Meta = (AllowPrivateAccess = true))
	float fZPos;

	
	
};
