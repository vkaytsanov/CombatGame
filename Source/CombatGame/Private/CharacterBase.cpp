// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"

#include "CharacterStats.h"
#include "MeleeAIController.h"
#include "Abillities/CharacterAbillity.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


// Sets default values
ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorEnableCollision(true);

	USkeletalMeshComponent* OwnerMesh = GetMesh();
	OwnerMesh->SetRelativeLocation(FVector(0, 0, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
	OwnerMesh->SetRelativeRotation(FRotator(0, -90.f, 0));

	Stats = CreateDefaultSubobject<UCharacterStats>(TEXT("CharacterStats"));
#if DEBUG_SHAPES
	GetCapsuleComponent()->SetHiddenInGame(false);
#endif
}

bool ACharacterBase::CanAttack() const
{
	UWorld* ThisWorld = GetWorld();
	check(ThisWorld);

	return LastAttackTime + GetAttackCooldownSeconds() < ThisWorld->GetTimeSeconds();
}

void ACharacterBase::TakeDamage(uint32 RawAmount)
{
	if (IsDead())
	{
		return;
	}

	UWorld* ThisWorld = GetWorld();
	check(ThisWorld);

	if (DamagedSFX)
	{
		UGameplayStatics::PlaySoundAtLocation(ThisWorld, DamagedSFX, GetActorLocation());
	}

	const uint32 TotalDamage = Stats->CalculateDamageToTake(RawAmount);


	if (CurrentHealth < TotalDamage)
	{
		CurrentHealth = 0;
		OnFatalDamageTaken();
	}
	else
	{
		CurrentHealth -= TotalDamage;
	}

	OnDamageTaken_BP(TotalDamage);
}

void ACharacterBase::OnFatalDamageTaken() const
{
	check(Controller);

	IKillableInterface* Killable = Cast<IKillableInterface>(Controller);
	checkf(Killable, TEXT("The controller must implemented IKillableInterface"));

	Killable->OnFatalDamageTaken();
}

void ACharacterBase::Heal(uint32 Amount)
{
	CurrentHealth = FMath::Min<uint32>(CurrentHealth + Amount, GetMaxHealth());
}

bool ACharacterBase::IsDead() const
{
	return CurrentHealth <= 0;
}

float ACharacterBase::GetAttackDamage() const
{
	return Stats->AttackDamage;
}

float ACharacterBase::GetCriticalStrikeChance() const
{
	return Stats->CriticalStrikeChance;
}

float ACharacterBase::GetLifeStealPercent() const
{
	return Stats->LifeStealPercent;
}

float ACharacterBase::GetMaxHealth() const
{
	return Stats->Health;
}

float ACharacterBase::GetCurrentHealth() const
{
	return CurrentHealth;
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	Stats->UpdateSecondaryStats();
	CurrentHealth = GetMaxHealth();
}

void ACharacterBase::PostInitProperties()
{
	Super::PostInitProperties();

	Abillities.Empty(AbillityTemplates.Num());
	for (TSubclassOf<UCharacterAbillity> AbillityTemplate : AbillityTemplates)
	{
		// Abillities.Add(NewObject<UCharacterAbillity>(GetTransientPackage(), *AbillityTemplate));
		Abillities.Add(NewObject<UCharacterAbillity>(this, *AbillityTemplate));
	}
}
