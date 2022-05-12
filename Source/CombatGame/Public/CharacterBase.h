// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

#define DEBUG_SHAPES 0

#ifndef DEBUG_SHAPES
#define DEBUG_SHAPES WITH_EDITORONLY_DATA
#endif

class UCharacterAbillity;


UCLASS(Abstract)
class COMBATGAME_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()
protected:
	UPROPERTY(Category = Character, EditAnywhere, BlueprintReadOnly)
	class UCharacterStats* Stats;

	UPROPERTY(Category = Character, EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<UCharacterAbillity>> AbillityTemplates;

	UPROPERTY(Category = Polishment, EditAnywhere)
	class USoundCue* DamagedSFX;
public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnAttackStart_BP();

	UFUNCTION(BlueprintImplementableEvent)
	void OnDamageTaken_BP(int32 DamageTaken);
public:
	ACharacterBase();
public:
	bool CanAttack() const;
	void TakeDamage(uint32 RawAmount);

	void OnFatalDamageTaken() const;
	virtual float GetAttackRange() const PURE_VIRTUAL(ACharacterBase::GetAttackRange, return 0.0f;)
	virtual bool HasAnyCharactersToAttack() const PURE_VIRTUAL(ACharacterBase::HasAnyCharactersToAttack, return false;)
	virtual void ExecuteAttack() PURE_VIRTUAL(ACharacterBase::ExecuteAttack, )
	virtual float GetAttackCooldownSeconds() const PURE_VIRTUAL(ACharacterBase::GetAttackCooldown, return 0.0f;)

	const TArray<UCharacterAbillity*>& GetAbillities() const { return Abillities; }
	void Heal(uint32 Amount);
	bool IsDead() const;
	float GetAttackDamage() const;
	float GetCriticalStrikeChance() const;
	float GetLifeStealPercent() const;

	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable)
	float GetCurrentHealth() const;

	virtual void BeginPlay() override;
	virtual void PostInitProperties() override;
protected:
	UPROPERTY(Transient)
	TArray<UCharacterAbillity*> Abillities;

	uint32 CurrentHealth;

	float LastAttackTime = FLT_MIN;

	uint32 bIsInCombat;
};
