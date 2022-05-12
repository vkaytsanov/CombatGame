// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterAbillity.h"
#include "FallingCubesAbillity.generated.h"

/**
 * 
 */
UCLASS()
class COMBATGAME_API UFallingCubesAbillity : public UCharacterAbillity
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	float SpawnHeight = 300.f;
public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> CubeToSpawn;
public:
	UFallingCubesAbillity() = default;
public:
	virtual void StartExecution(AController* Controller) override;
	virtual void Update(AController* Controller) override {}
private:
	void OnExecutionTimerEnd();
private:
	UPROPERTY(Transient)
	TArray<AActor*> SpawnedCubes;
};
