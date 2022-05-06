// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeCharacter.h"

#include "MeleeAIController.h"
#include "Weapon.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogMeleeCharacter, Warning, All);

AMeleeCharacter::AMeleeCharacter()
{
	float CapsuleRadius = GetCapsuleComponent()->GetScaledCapsuleRadius();
	WeaponAABB = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponAABB"));
	WeaponAABB->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	WeaponAABB->SetRelativeLocation(FVector(CapsuleRadius * 0.5f, CapsuleRadius, 0));
	WeaponAABB->AreaClass = nullptr;

#if WITH_EDITORONLY_DATA
	WeaponAABB->SetHiddenInGame(false);
#endif

	AIControllerClass = AMeleeAIController::StaticClass();
}

void AMeleeCharacter::PostInitProperties()
{
	Super::PostInitProperties();

	float CapsuleRadius = GetCapsuleComponent()->GetScaledCapsuleRadius();
	float WeaponRange = 0.0f;

	if (WeaponTemplateClass)
	{
		UWorld* ThisWorld = GetWorld();
		check(ThisWorld);

		if (!WeaponSocketName.IsValid())
		{
			UE_LOG(LogMeleeCharacter, Warning, TEXT("Missing specified Socket to attach for the Weapon."));
			return;
		}

		bool bDidAttach = false;
		if (!Weapon)
		{
			USkeletalMeshComponent* ActorMesh = GetMesh();

			Weapon = ThisWorld->SpawnActor<AWeapon>(WeaponTemplateClass.Get());
			bDidAttach = Weapon->AttachToSocket(ActorMesh, ActorMesh->GetSocketByName(WeaponSocketName));
		}

		if (!bDidAttach)
		{
			UE_LOG(LogMeleeCharacter, Warning, TEXT("Couldn't attach Weapon to Socket %s"), *WeaponSocketName.ToString());
			return;
		}

		WeaponRange = Weapon->AttackRange;
	}

	WeaponAABB->InitBoxExtent(FVector(CapsuleRadius, CapsuleRadius, WeaponRange));
	WeaponAABB->SetRelativeLocation(FVector(CapsuleRadius * 2, 0, 0));
}
