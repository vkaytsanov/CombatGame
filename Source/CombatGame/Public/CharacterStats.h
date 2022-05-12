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
	UCharacterStats();
public:
	void UpdateSecondaryStats();
	uint32 CalculateDamageToTake(uint32 RawAmount) const;
	virtual void PostInitProperties() override;

#if WITH_EDITORONLY_DATA
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
