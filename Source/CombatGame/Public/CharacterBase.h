// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "CharacterBase.generated.h"

UCLASS(Abstract)
class COMBATGAME_API ACharacterBase : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(Category = Character, EditAnywhere)
	class UCharacterStatsComponent* StatsComponent;

	UPROPERTY(Category = Polishment, EditAnywhere)
	class USoundCue* DamagedSFX;
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnDamageTaken_BP(int32 DamageTaken);

public:
	// Sets default values for this pawn's properties
	ACharacterBase();
public:
	virtual void OnFatalDamageTaken();

	void TakeDamage(uint32 RawAmount);

	virtual FGenericTeamId GetGenericTeamId() const override;
};
