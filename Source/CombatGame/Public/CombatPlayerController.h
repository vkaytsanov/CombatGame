// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CombatPlayerState.h"
#include "Killable.h"
#include "CombatPlayerController.generated.h"



#define COMBAT_PLAYER_STATES(MACRO)   \
    MACRO(Idle)                       \
    MACRO(Jump)                       \
    MACRO(Attack)                     \
    MACRO(UsingAbillity)              \

#define GENERATE_PLAYER_STATE_ENUMS(State) CPS_##State##Action,

#define GENERATE_PLAYER_STATE_STRUCTS(State) F##State##PlayerState State##PlayerState;

enum ECurrentCombatPlayerState : uint8
{
	COMBAT_PLAYER_STATES(GENERATE_PLAYER_STATE_ENUMS)
};


/**
 * 
 */
UCLASS()
class COMBATGAME_API ACombatPlayerController : public APlayerController, public IKillableInterface
{
	GENERATED_BODY()
public:
	ACombatPlayerController();
public:
	virtual void OnFatalDamageTaken() override;
	void SetCurrentState(ECurrentCombatPlayerState State);
	void UpdateMovement();

	class UCameraComponent* GetAttachedCamera() const
	{
		check(PlayerCamera);
		return PlayerCamera;
	}

	class ACharacterBase* GetCharacterBase() const
	{
		check(OwnerPlayer);
		return OwnerPlayer;
	}

	void SetCurrentRunningAbillity(class UCharacterAbillity* Abillity)
	{
		CurrentRunningAbillity = Abillity;
	}

	class UCharacterAbillity* GetCurrentRunningAbillity() const
	{
		return CurrentRunningAbillity;
	}

	UFUNCTION(BlueprintCallable)
	bool IsAttacking() const;

	UFUNCTION(BlueprintCallable)
	bool ConditionalAdvanceAttack();

	UFUNCTION(BlueprintCallable)
	void OnAttackEnd_BP();
protected:
	virtual void OnPossess(APawn* aPawn) override;
protected:
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaTime) override;
private:
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
private:
	UPROPERTY()
	class ACharacterBase* OwnerPlayer;

	UPROPERTY()
	class UCameraComponent* PlayerCamera;

	UPROPERTY()
	class UCharacterAbillity* CurrentRunningAbillity;

	FCombatPlayerState* CurrentPlayerState;

	COMBAT_PLAYER_STATES(GENERATE_PLAYER_STATE_STRUCTS);
};


#undef GENERATE_PLAYER_STATE_ENUMS
#undef GENERATE_PLAYER_STATE_STRUCTS
