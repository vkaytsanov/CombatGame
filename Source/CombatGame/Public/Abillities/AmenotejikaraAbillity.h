// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterAbillity.h"
#include "AmenotejikaraAbillity.generated.h"

/**
 *  Copyright Masashi Kishimoto, Naruto
 *
 *  Switches places with an aimed at object in the scene.
 *  Preserves rotations and physics.
 */
UCLASS()
class COMBATGAME_API UAmenotejikaraAbillity : public UCharacterAbillity
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	float AmenotejikaraRadius = 1500.f;

	UPROPERTY(EditAnywhere)
	float TimeDilationWhenUsed = 0.05f;

	UPROPERTY(EditAnywhere)
	float ToleranceToBeATargetInRadians = 3.1415926535897932f / 14;

	UPROPERTY(VisibleAnywhere, Transient)
	class USphereComponent* AmenotejikaraSphere;

	UPROPERTY(EditAnywhere)
	class UMaterial* MarkAsSelectedMaterial;
public:
	UAmenotejikaraAbillity();
public:
	virtual void OnPosses(AController* Controller) override;
	virtual void StartExecution(AController* Controller) override;
	virtual void FinishExecution(AController* Controller) override;
	virtual void Update(AController* Controller) override;
	virtual bool HasFinishedExecution() const override;

	virtual void BeginDestroy() override;

	UFUNCTION()
	void OnActorOverlapWithAmenotejikaraSphere(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnActorEndOverlapWithAmenotejikaraSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
private:
	void SwapMaterials(AActor* WithActor);
private:
	UPROPERTY(Transient)
	TArray<AActor*> ActorsInRange;

	UPROPERTY(Transient)
	AActor* BestToSwapPlaces = nullptr;

	UPROPERTY(Transient)
	UMaterialInterface* BestToSwapPlacesOriginalMaterial = nullptr;

	FVector LastForwardVector = FVector(FLT_MIN, FLT_MIN, FLT_MIN);
};
