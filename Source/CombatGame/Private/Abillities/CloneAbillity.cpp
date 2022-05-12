// Fill out your copyright notice in the Description page of Project Settings.


#include "Abillities/CloneAbillity.h"

#include "AIController.h"
#include "CharacterBase.h"
#include "CloneAIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UCloneAbillity::UCloneAbillity()
{
	// Triangle Formation
	ClonesSpawnOffsets[0] = FVector(-200, 0, 0);
	ClonesSpawnOffsets[1] = FVector(200, 0, 0);
	ClonesSpawnOffsets[2] = FVector(0, 200, 0);
}

void UCloneAbillity::OnPosses(AController* Controller)
{
	Super::OnPosses(Controller);

	ACharacter* OwnerCharacter = Controller->GetCharacter();
	check(OwnerCharacter);

	UPrimitiveComponent* CharacterRootComponent = Cast<UPrimitiveComponent>(OwnerCharacter->GetRootComponent());
	check(CharacterRootComponent);

	UMaterialInterface* OwnerCharcaterMaterial = OwnerCharacter->GetMesh()->GetMaterial(0);
	check(OwnerCharcaterMaterial);

	auto GetFirstTextureFromProperty = [&OwnerCharcaterMaterial](EMaterialProperty Property)
	{
		TArray<UTexture*> UsedTextures;
		OwnerCharcaterMaterial->GetTexturesInPropertyChain(Property, UsedTextures, nullptr, nullptr, ERHIFeatureLevel::SM5, EMaterialQualityLevel::Medium);
		check(UsedTextures.Num() >= 1);

		return UsedTextures[0];
	};

	OwnerCharacterAlbedoTexture = GetFirstTextureFromProperty(MP_BaseColor);
	OwnerCharacterNormalTexture = GetFirstTextureFromProperty(MP_Normal);
	OwnerCharacterSpecularTexture = GetFirstTextureFromProperty(MP_Specular);
}

void UCloneAbillity::StartExecution(AController* Controller)
{
	Super::StartExecution(Controller);

	ACharacter* OwnerCharacter = Controller->GetCharacter();
	check(OwnerCharacter);

	UWorld* ThisWorld = OwnerCharacter->GetWorld();
	check(ThisWorld);

	const FVector& SpawnLocation = OwnerCharacter->GetActorLocation();
	const FRotator& SpawnRotation = OwnerCharacter->GetActorRotation();

	for (uint32 i = 0; i < ClonesToSpawn; i++)
	{
		const FVector& SpawnLocationOffset = SpawnLocation + ClonesSpawnOffsets[i].GetSafeNormal() * 1.f;

		FActorSpawnParameters ActorSpawnInfo;
		ActorSpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ActorSpawnInfo.OverrideLevel = OwnerCharacter->GetLevel();
		ActorSpawnInfo.ObjectFlags |= RF_Transient;
		ACharacter* NewActor = ThisWorld->SpawnActor<ACharacter>(OwnerCharacter->GetClass(), SpawnLocationOffset, SpawnRotation, ActorSpawnInfo);

		UMaterialInstanceDynamic* NewActorMaterial = UMaterialInstanceDynamic::Create(CloneMaterial, NewActor);
		NewActorMaterial->SetTextureParameterValue(MaterialTextureAlbedoParameterName, OwnerCharacterAlbedoTexture);
		NewActorMaterial->SetTextureParameterValue(MaterialTextureNormalParameterName, OwnerCharacterNormalTexture);
		NewActorMaterial->SetTextureParameterValue(MaterialTextureSpecularParameterName, OwnerCharacterSpecularTexture);
		NewActorMaterial->SetScalarParameterValue(MaterialScalarActorSpawnTimeParameterName, ThisWorld->GetTimeSeconds());
		NewActor->GetMesh()->SetMaterial(0, NewActorMaterial);

		FActorSpawnParameters ControllerSpawnInfo;
		ControllerSpawnInfo.Instigator = NewActor->GetInstigator();
		ControllerSpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ControllerSpawnInfo.OverrideLevel = NewActor->GetLevel();
		ControllerSpawnInfo.ObjectFlags |= RF_Transient;
		ACloneAIController* CloneController = GetWorld()->SpawnActor<ACloneAIController>(ACloneAIController::StaticClass(), SpawnLocationOffset, SpawnRotation, ControllerSpawnInfo);
		CloneController->SetAbillityInstigator(Cast<ACharacterBase>(OwnerCharacter));
		CloneController->SetFollowingOffset(ClonesSpawnOffsets[i]);
		CloneController->AttachToActor(NewActor, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		CloneController->Possess(NewActor);
		CloneController->MoveToLocation(SpawnLocation + ClonesSpawnOffsets[i], 0, false, false, false, false);
		SpawnedActors.Add(NewActor);
	}

	FTimerHandle Handle;
	ThisWorld->GetTimerManager().SetTimer(Handle, this, &UCloneAbillity::OnExecutionTimerEnd, MaxExecutionTime, false);
}

void UCloneAbillity::Update(AController* Controller)
{}

void UCloneAbillity::OnExecutionTimerEnd()
{
	for (ACharacter* Character : SpawnedActors)
	{
		if (Character)
		{
			Character->Destroy();
		}
	}
}
