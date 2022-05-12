// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "MageCharacter.generated.h"

UCLASS()
class COMBATGAME_API AMageCharacter : public ACharacterBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMageCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
