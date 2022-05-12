// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CharacterAbillity.generated.h"

class AController;
class ACharacterBase;


UENUM()
enum EAbillityExecutionType
{
	AE_Instant UMETA(DisplayName = "Instant"),
	AE_Targetable UMETA(DisplayName = "Targetable"),
};

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class COMBATGAME_API UCharacterAbillity : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TEnumAsByte<EAbillityExecutionType> ExecutionType = AE_Instant;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "ExecutionType"))
	float MaxExecutionTime = 0.0f;

	UPROPERTY(EditAnywhere)
	float Cooldown = 1.0f;

	UPROPERTY(EditAnywhere)
	FKey ActivationKey = FName(NAME_None);

	UPROPERTY(EditAnywhere)
	bool bCanMoveWhenExecuting = false;

	UPROPERTY(EditAnywhere)
	bool bCanAttackWhenExecuting = false;

	UPROPERTY(EditAnywhere)
	bool bCanJumpWhenExecuting = false;

	float LastUseTime = FLT_MIN;

public:
	UCharacterAbillity() = default;
public:
	virtual void OnPosses(AController* Controller) {}
	virtual void StartExecution(AController* Controller);
	virtual void FinishExecution(AController* Controller);
	virtual void Update(AController* Controller) PURE_VIRTUAL(UCharacterAbillity::Update, )

	bool IsTargetable() const { return ExecutionType == AE_Targetable;}
	float GetTimeLeftUntilUsable() const;
	bool IsExecutable() const;
	virtual bool HasFinishedExecution() const;
};
