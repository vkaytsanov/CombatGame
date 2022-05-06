// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"

#include "CharacterStatsComponent.h"
#include "CombatPlayerController.h"
#include "MeleeAIController.h"
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
	OwnerMesh->SetCollisionObjectType(ECC_GameTraceChannel1);

	StatsComponent = CreateDefaultSubobject<UCharacterStatsComponent>(TEXT("CharacterStatsComponent"));
	StatsComponent->FatalDamageListener.BindUObject(this, &ACharacterBase::OnFatalDamageTaken);

#if WITH_EDITORONLY_DATA
	GetCapsuleComponent()->SetHiddenInGame(false);
#endif
}

void ACharacterBase::TakeDamage(uint32 RawAmount)
{
	UWorld* ThisWorld = GetWorld();
	check(ThisWorld);

	if (DamagedSFX)
	{
		UGameplayStatics::PlaySoundAtLocation(ThisWorld, DamagedSFX, GetActorLocation());
	}

	const uint32 TotalDamage = StatsComponent->CalculateDamageToTake(RawAmount);

	StatsComponent->TakeDamage(TotalDamage);
	OnDamageTaken_BP(TotalDamage);
}

void ACharacterBase::OnFatalDamageTaken()
{
	if (Controller->IsA<APlayerController>())
	{
		ACombatPlayerController* CombatController = Cast<ACombatPlayerController>(Controller);
		checkf(CombatController, TEXT("PlayerController Type Mismatch"));
		CombatController->OnFatalDamageTaken();
	}
	else
	{
		AMeleeAIController* AIController = Cast<AMeleeAIController>(Controller);
		checkf(AIController, TEXT("AIController Type Mismatch"));
		// AIController->On
		// TODO:
		// Drop Item
		// ..

		// Give player exp
		// ..
	}
}


FGenericTeamId ACharacterBase::GetGenericTeamId() const
{
	static const FGenericTeamId PlayerTeam(0);
	static const FGenericTeamId AITeam(1);
	return Controller->IsA<APlayerController>() ? PlayerTeam : AITeam;
}
