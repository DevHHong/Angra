// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "TowerofAngraCharacter.h"
//#include "GameFramework/Character.h"
#include "HKayaCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnSkillEndDelegate);

UCLASS()
class TOWEROFANGRA_API AHKayaCharacter : public ATowerofAngraCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AHKayaCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void InitCommon() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Attack() override;
	virtual void Skill() override;
	FOnSkillEndDelegate OnSkillEnd;
private:
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void AttackStartComboState();
	void AttackEndComboState();

	void AttackCheck();		// 충돌체크

	// Skill
	UFUNCTION()
	void OnSkillMontageEnded(UAnimMontage* Montage, bool bInterrupted);

public:
	UPROPERTY()
	class UHKayaAnimInstance* KayaAnim;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UCameraShake> CamEffect;
public:
	UPROPERTY(VisibleAnywhere, Category = Effect)
	class UParticleSystemComponent* SkillEffect;

	void SkillCheck();		// 스킬 충돌체크
};
