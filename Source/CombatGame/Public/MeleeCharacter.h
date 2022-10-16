// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "MeleeCharacter.generated.h"

/**
 * 
 */
UCLASS()
class COMBATGAME_API AMeleeCharacter : public ACharacterBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> WeaponTemplateClass;

	UPROPERTY(EditAnywhere)
	class AWeapon* Weapon;

	UPROPERTY(EditAnywhere)
	FName WeaponSocketName;

	UPROPERTY(VisibleAnywhere, Transient)
	class UBoxComponent* WeaponAABB;
public:
	AMeleeCharacter();
public:
	virtual float GetAttackRange() const override;
	virtual bool HasAnyCharactersToAttack() const override { return !!CharactersInWeaponRange.Num(); }
	virtual float GetAttackCooldownSeconds() const override;
	virtual void ExecuteAttack() override;
	virtual void PostInitProperties() override;
	virtual void UnPossessed() override;

	static float GetDefaultAttackCooldown();
	static float GetDefaultAttackRange();
protected:
	UFUNCTION()
	void OnWeaponOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnWeaponEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
protected:
	UPROPERTY(VisibleAnywhere)
	TArray<ACharacterBase*> CharactersInWeaponRange;
};
