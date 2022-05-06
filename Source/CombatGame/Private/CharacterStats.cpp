// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStats.h"


void UCharacterStats::PostInitProperties()
{
	Super::PostInitProperties();

	UpdateSecondaryStats();
}

void UCharacterStats::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	UpdateSecondaryStats();
}

void UCharacterStats::UpdateSecondaryStats()
{
	AttackDamage = Strength * 2 - 20;
	Health = Stamina * 10;

	CriticalStrikeChance = CriticalStrike * 0.22f;
	LifeStealPercent = LifeSteal * 0.07f;
}
