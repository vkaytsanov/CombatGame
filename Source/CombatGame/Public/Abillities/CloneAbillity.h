// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterAbillity.h"
#include "CloneAbillity.generated.h"


class UTexture;

/* Creates clones of the possessed actor */
UCLASS()
class COMBATGAME_API UCloneAbillity : public UCharacterAbillity
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, meta = (ClampMin = 1, ClampMax = 3))
	uint32 ClonesToSpawn = 3;

	UPROPERTY(EditAnywhere)
	FVector ClonesSpawnOffsets[3];

	UPROPERTY(EditAnywhere)
	UMaterial* CloneMaterial;

	UPROPERTY(EditAnywhere)
	FName MaterialTextureAlbedoParameterName;

	UPROPERTY(EditAnywhere)
	FName MaterialTextureNormalParameterName;

	UPROPERTY(EditAnywhere)
	FName MaterialTextureSpecularParameterName;

	UPROPERTY(EditAnywhere)
	FName MaterialScalarActorSpawnTimeParameterName;
public:
	UCloneAbillity();
public:
	virtual void OnPosses(AController* Controller) override;
	virtual void StartExecution(AController* Controller) override;
	virtual void Update(AController* Controller) override;
private:
	void OnExecutionTimerEnd();
private:
	UPROPERTY(Transient)
	UTexture* OwnerCharacterAlbedoTexture;

	UPROPERTY(Transient)
	UTexture* OwnerCharacterNormalTexture;

	UPROPERTY(Transient)
	UTexture* OwnerCharacterSpecularTexture;

	TArray<ACharacter*, TInlineAllocator<4>> SpawnedActors;
};