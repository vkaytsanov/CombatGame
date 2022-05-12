// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStats.h"


UCharacterStats::UCharacterStats()
{
	UpdateSecondaryStats();
}

uint32 UCharacterStats::CalculateDamageToTake(uint32 RawAmount) const
{
	// If there are any damage mitigation effects in the future
	return RawAmount;
}

void UCharacterStats::PostInitProperties()
{
	Super::PostInitProperties();

	UpdateSecondaryStats();
}

#if WITH_EDITORONLY_DATA
void UCharacterStats::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	UpdateSecondaryStats();
}
#endif

void UCharacterStats::UpdateSecondaryStats()
{
	AttackDamage = FMath::RoundToFloat(Strength * 1.8f);
	Health = 20 + Stamina * 10.f;

	CriticalStrikeChance = CriticalStrike * 0.22f;
	LifeStealPercent = LifeSteal * 0.07f;
}
