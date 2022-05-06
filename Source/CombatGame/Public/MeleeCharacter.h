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

	UPROPERTY(VisibleAnywhere, Transient)
	class AWeapon* Weapon;

	UPROPERTY(EditAnywhere)
	FName WeaponSocketName;

	UPROPERTY(VisibleAnywhere, Transient)
	class UBoxComponent* WeaponAABB;
public:
	AMeleeCharacter();
public:
	virtual void PostInitProperties() override;
};
