// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CombatPlayerState.h"
#include "CombatPlayerController.generated.h"



#define COMBAT_PLAYER_STATES(MACRO)   \
    MACRO(Idle)                       \
    MACRO(Jump)                       \
    MACRO(Attack)                     \
    MACRO(Amenotejikara)              \

#define GENERATE_PLAYER_STATE_ENUMS(State) CPS_##State##Action,

#define GENERATE_PLAYER_STATE_STRUCTS(State) F##State##ActionState State##PlayerState;

enum ECurrentCombatPlayerState : uint8
{
	COMBAT_PLAYER_STATES(GENERATE_PLAYER_STATE_ENUMS)
};


/**
 * 
 */
UCLASS()
class COMBATGAME_API ACombatPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	float AmenotejikaraRadius = 1500.f;
public:
	ACombatPlayerController();
public:
	void OnFatalDamageTaken();
	void SetCurrentState(ECurrentCombatPlayerState State);
	void MoveForward(float Value);
	void MoveRight(float Value);

	class UCameraComponent* GetAttachedCamera() const
	{
		check(PlayerCamera);
		return PlayerCamera;
	}
	virtual void BeginDestroy() override;
protected:
	virtual void OnPossess(APawn* aPawn) override;
	virtual void BeginPlay() override;
protected:
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaTime) override;
private:
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	UFUNCTION()
	void OnActorOverlapWithAmenotejikaraSphere(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnActorEndOverlapWithAmenotejikaraSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
private:
	UPROPERTY()
	class UCameraComponent* PlayerCamera;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* AmenotejikaraSphere;

	FCombatPlayerState* CurrentPlayerState;

	COMBAT_PLAYER_STATES(GENERATE_PLAYER_STATE_STRUCTS);
};


#undef GENERATE_PLAYER_STATE_ENUMS
#undef GENERATE_PLAYER_STATE_STRUCTS
