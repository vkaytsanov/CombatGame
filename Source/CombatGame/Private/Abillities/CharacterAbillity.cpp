// Fill out your copyright notice in the Description page of Project Settings.


#include "Abillities/CharacterAbillity.h"

void UCharacterAbillity::StartExecution(AController* Controller)
{
	LastUseTime = GetWorld()->GetAudioTimeSeconds();
}

void UCharacterAbillity::FinishExecution(AController* Controller)
{
	LastUseTime = GetWorld()->GetAudioTimeSeconds();
}

float UCharacterAbillity::GetTimeLeftUntilUsable() const
{
	float TimeLeft = LastUseTime + Cooldown - GetWorld()->GetAudioTimeSeconds();
	return TimeLeft > 0 ? TimeLeft : 0;
}

bool UCharacterAbillity::IsExecutable() const
{
	return !GetTimeLeftUntilUsable();
}

bool UCharacterAbillity::HasFinishedExecution() const
{
	return LastUseTime + MaxExecutionTime < GetWorld()->GetAudioTimeSeconds();
}
