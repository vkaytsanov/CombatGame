// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class COMBATGAME_API AWeapon : public AActor
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	float AttackRange = 75.f;

	UPROPERTY(EditAnywhere)
	float AttackCooldown = 2.f;
public:	
	// Sets default values for this actor's properties
	AWeapon();
public:
	virtual void PostInitProperties() override;
	bool AttachToSocket(USkeletalMeshComponent* ActorMesh, const USkeletalMeshSocket* Socket);
};
