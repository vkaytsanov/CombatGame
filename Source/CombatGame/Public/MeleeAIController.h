// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Killable.h"
#include "CombatEnemyState.h"
#include "MeleeAIController.generated.h"


#define COMBAT_ENEMY_STATE(MACRO)     \
    MACRO(Idle)                       \
    MACRO(Wander)                     \
    MACRO(Chase)                      \
    MACRO(Attack)                     \
    MACRO(ReturnHome)                 \
    MACRO(UsingAbillity)              \




class UCharacterAbillity;

enum ECurrentCombatEnemyState : uint8
{
#define GENERATE_ENEMY_STATE_ENUMS(State) CES_##State##Action,
	COMBAT_ENEMY_STATE(GENERATE_ENEMY_STATE_ENUMS)
#undef GENERATE_ENEMY_STATE_ENUMS
};

/**
 * 
 */
UCLASS()
class COMBATGAME_API AMeleeAIController : public AAIController, public IKillableInterface
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, Category = "Targeting", Transient)
	class USphereComponent* HearingSphere;

	UPROPERTY(VisibleAnywhere, Category = "Targeting", Transient)
	class USphereComponent* SightSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
	float HearingRadius = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
	float SightRadius = 1800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting", meta = (ClampMax = 90.f))
	float SightAngle = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wandering")
	float WanderRadius = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wandering")
	float WanderMovementSpeed = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wandering")
	float WanderDelaySeconds = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chasing")
	float MaxChaseDistance = 2500.f;
public:
	AMeleeAIController();
public:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnFatalDamageTaken() override;
	void SetCurrentState(ECurrentCombatEnemyState State);

	class ACharacterBase* GetCharacterBase() const
	{
		check(CharacterBase);
		return CharacterBase;
	}

	ACharacterBase* GetCurrentTarget() const
	{
		return CurrentTarget;
	}

	const FVector& GetChaseStartLocation() const
	{
		return ChaseStartLocation;
	}

	void SetCurrentRunningAbillity(UCharacterAbillity* Abillity)
	{
		CurrentAbillity = Abillity;
	}

	UCharacterAbillity* GetCurrentRunningAbillity() const
	{
		return CurrentAbillity;
	}
protected:
	virtual void BeginDestroy() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void OnHearingOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSightOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSensesEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void SetTarget(class ACharacterBase* Target);
protected:
	UPROPERTY(VisibleAnywhere, Transient)
	class ACharacterBase* CurrentTarget = nullptr;

	UPROPERTY(VisibleAnywhere, Transient)
	FVector ChaseStartLocation;

	UPROPERTY(VisibleAnywhere, Transient)
	FVector	SpawnLocation;

	UPROPERTY(VisibleAnywhere, Transient)
	float SpawnMaxSpeed;

	float LastWanderTime = FLT_MAX;

	/** CharacterBase currently being controlled by this controller.  Value is same as Character */
	UPROPERTY(Transient);
	ACharacterBase* CharacterBase;

	UPROPERTY(Transient);
	UCharacterAbillity* CurrentAbillity;

	FCombatEnemyState* CurrentEnemyState;

#define GENERATE_ENEMY_STATE_STRUCTS(State) F##State##EnemyState State##EnemyState;
	COMBAT_ENEMY_STATE(GENERATE_ENEMY_STATE_STRUCTS);
#undef GENERATE_ENEMY_STATE_STRUCTS
};


