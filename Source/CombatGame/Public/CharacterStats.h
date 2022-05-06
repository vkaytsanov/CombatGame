// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterStats.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class COMBATGAME_API UCharacterStats : public UDataAsset
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	uint32 Strength;

	UPROPERTY(EditAnywhere)
	uint32 Stamina;

	UPROPERTY(EditAnywhere)
	uint32 CriticalStrike;

	UPROPERTY(EditAnywhere)
	uint32 LifeSteal;
public:
	UPROPERTY(VisibleAnywhere, Transient)
	float AttackDamage;

	UPROPERTY(VisibleAnywhere, Transient)
	float Health;

	UPROPERTY(VisibleAnywhere, Transient)
	float CriticalStrikeChance;

	UPROPERTY(VisibleAnywhere, Transient)
	float LifeStealPercent;
public:
	virtual void PostInitProperties() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
private:
	void UpdateSecondaryStats();
};
