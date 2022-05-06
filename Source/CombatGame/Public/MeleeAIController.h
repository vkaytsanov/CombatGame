// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MeleeAIController.generated.h"

/**
 * 
 */
UCLASS()
class COMBATGAME_API AMeleeAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AMeleeAIController();
public:
	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION(BlueprintCallable)
	void SetReturningHome();

	UFUNCTION(BlueprintCallable)
	void OnReturnedHome();

	UPROPERTY(VisibleAnywhere, Category = "Targeting")
	class USphereComponent* HearingSphere;

	UPROPERTY(VisibleAnywhere, Category = "Targeting")
	class USphereComponent* SightSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
	float HearingRadius = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
	float SightRadius = 1800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting", meta = (ClampMax = 90.f))
	float SightAngle = 75.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
	float AttackRadius = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wandering")
	float WanderRadius = 500.f;
protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void OnHearingOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSightOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSensesEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void SetTarget(class ACharacterBase* Target);

	void Wander();
protected:
	UPROPERTY(VisibleAnywhere, Transient)
	class ACharacterBase* CurrentTarget = nullptr;

	UPROPERTY(VisibleAnywhere, Transient)
	FVector ChaseStartLocation;

	UPROPERTY(VisibleAnywhere, Transient)
	FVector	SpawnLocation;

	UPROPERTY(VisibleAnywhere, Transient)
	float SpawnMaxSpeed;
};
