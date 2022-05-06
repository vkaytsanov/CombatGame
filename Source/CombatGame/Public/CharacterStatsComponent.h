// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterStats.h"
#include "Components/ActorComponent.h"
#include "CharacterStatsComponent.generated.h"


class UCharacterStats;


DECLARE_DELEGATE_RetVal(void, FatalDamageDelegate);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COMBATGAME_API UCharacterStatsComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	FatalDamageDelegate FatalDamageListener;
public:
	// Sets default values for this component's properties
	UCharacterStatsComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	void Heal(uint32 Amount);
	uint32 CalculateDamageToTake(uint32 RawAmount) const;
	void TakeDamage(uint32 Amount);

	bool IsDead() const
	{
		return CurrentHealth <= 0;
	}

	float GetAttackDamage() const
	{
		return Stats->AttackDamage;
	}

	float GetCriticalStrikeChance() const
	{
		return Stats->CriticalStrikeChance;
	}

	float GetLifeStealPercent() const
	{
		return Stats->LifeStealPercent;
	}

	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() const
	{
		return Stats->Health;
	}

	UFUNCTION(BlueprintCallable)
	float GetCurrentHealth() const
	{
		return CurrentHealth;
	}
private:
	UPROPERTY()
	UCharacterStats* Stats;

	uint32 CurrentHealth;
};
