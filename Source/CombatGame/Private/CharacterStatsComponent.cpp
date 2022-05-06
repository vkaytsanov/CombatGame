// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStatsComponent.h"

#include "CharacterStats.h"

// Sets default values for this component's properties
UCharacterStatsComponent::UCharacterStatsComponent()
{
	Stats = CreateDefaultSubobject<UCharacterStats>(TEXT("CharacterStats"));
}


// Called when the game starts
void UCharacterStatsComponent::BeginPlay()
{
	check(Stats);

	CurrentHealth = GetMaxHealth();

	Super::BeginPlay();
}

uint32 UCharacterStatsComponent::CalculateDamageToTake(uint32 RawAmount) const
{
	// If there is armor or other damage mitigation modifiers
	return RawAmount;
}

void UCharacterStatsComponent::Heal(uint32 Amount)
{
	CurrentHealth = FMath::Min<uint32>(CurrentHealth + Amount, GetMaxHealth());
}

void UCharacterStatsComponent::TakeDamage(uint32 Amount)
{
	if (CurrentHealth < Amount)
	{
		CurrentHealth = 0;

		FatalDamageListener.Execute();
	}
	else
	{
		CurrentHealth -= Amount;
	}
}
