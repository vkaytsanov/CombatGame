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
	WeaponAABB->AreaClass = nullptr;
	WeaponAABB->SetCollisionProfileName("TriggerPawn");
	WeaponAABB->CanCharacterStepUpOn = ECB_No;
	WeaponAABB->bShouldCollideWhenPlacing = false;
	WeaponAABB->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	WeaponAABB->SetRelativeLocation(FVector(CapsuleRadius * 0.5f, CapsuleRadius, 0));
	WeaponAABB->OnComponentBeginOverlap.AddDynamic(this, &AMeleeCharacter::OnWeaponOverlap);
	WeaponAABB->OnComponentEndOverlap.AddDynamic(this, &AMeleeCharacter::OnWeaponEndOverlap);

#if DEBUG_SHAPES
	WeaponAABB->SetHiddenInGame(false);
#endif

	AIControllerClass = AMeleeAIController::StaticClass();
}

float AMeleeCharacter::GetAttackRange() const
{
	if (Weapon)
	{
		return Weapon->AttackRange;
	}
	return 60.0f;
}

float AMeleeCharacter::GetAttackCooldownSeconds() const
{
	if (Weapon)
	{
		return Weapon->AttackCooldown;
	}
	return 2.0f;
}

void AMeleeCharacter::ExecuteAttack()
{
	LastAttackTime = GetWorld()->GetTimeSeconds();

	float BaseDamage = GetAttackDamage();
	for (int32 i = 0; i < CharactersInWeaponRange.Num(); i++)
	{
		ACharacterBase* Character = CharactersInWeaponRange[i];
		bool bIsCritical = GetCriticalStrikeChance() >= (FMath::FRand() * 100);
		float CurrentDamage = BaseDamage + BaseDamage * bIsCritical;
		UE_LOG(LogMeleeCharacter, Log, TEXT("%s did %hs Attack On %s, Damage %f"), *GetFName().ToString(), bIsCritical ? "Critical" : "Normal", *Character->GetFName().ToString(), CurrentDamage);
		Character->TakeDamage(CurrentDamage);
	}

	if (CharactersInWeaponRange.Num())
	{
		bIsInCombat = true;
	}
}

void AMeleeCharacter::PostInitProperties()
{
	Super::PostInitProperties();

	float CapsuleRadius = GetCapsuleComponent()->GetScaledCapsuleRadius();
	float WeaponRange = 60.0f;

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

			const USkeletalMeshSocket* WeaponSocket = ActorMesh->GetSocketByName(WeaponSocketName);
			if (!WeaponSocket)
			{
				UE_LOG(LogMeleeCharacter, Warning, TEXT("There isn't an existing socket for a Weapon"));
				return;
			}

			Weapon = ThisWorld->SpawnActor<AWeapon>(WeaponTemplateClass.Get());
			bDidAttach = Weapon->AttachToSocket(ActorMesh, WeaponSocket);
		}

		if (!bDidAttach)
		{
			UE_LOG(LogMeleeCharacter, Warning, TEXT("Couldn't attach Weapon to Socket %s"), *WeaponSocketName.ToString());
			return;
		}

		WeaponRange = Weapon->AttackRange;
	}

	WeaponAABB->InitBoxExtent(FVector(WeaponRange, WeaponRange, WeaponRange));
	WeaponAABB->SetRelativeLocation(FVector(CapsuleRadius * 2, 0, 0));
}

void AMeleeCharacter::UnPossessed()
{
	Super::UnPossessed();
	if (Weapon)
	{
		Weapon->Destroy();
	}
}

void AMeleeCharacter::OnWeaponOverlap(UPrimitiveComponent*,
                                      AActor* Other,
                                      UPrimitiveComponent*,
                                      int32,
                                      bool,
                                      const FHitResult&)
{
	// If it is newly spawned and doesn't have controller yet;
	if (!GetController() | !Other->GetInstigatorController())
	{
		return;
	}

	// Do not consider characters from the same race as enemies
	if (GetClass() == Other->GetClass())
	{
		return;
	}

	
	ACharacterBase* OtherCharacter = Cast<ACharacterBase>(Other);
	if (OtherCharacter)
	{
		CharactersInWeaponRange.Add(OtherCharacter);
	}
}

void AMeleeCharacter::OnWeaponEndOverlap(UPrimitiveComponent*,
                                         AActor* OtherActor,
                                         UPrimitiveComponent*,
                                         int32)
{
	// Do not consider characters from the same race as enemies
	if (GetClass() == OtherActor->GetClass())
	{
		return;
	}

	ACharacterBase* OtherCharacter = Cast<ACharacterBase>(OtherActor);
	if (OtherCharacter)
	{
		CharactersInWeaponRange.RemoveSingleSwap(OtherCharacter);
	}
}
