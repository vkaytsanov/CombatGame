// Fill out your copyright notice in the Description page of Project Settings.


#include "Abillities/AmenotejikaraAbillity.h"
#include "CombatPlayerController.h"

#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"


DEFINE_LOG_CATEGORY_STATIC(LogPlayerState, Warning, All);


namespace 
{
void SetCameraRotationLag(AController* Controller, bool bIsEnabled)
{
	ACombatPlayerController* Player = Cast<ACombatPlayerController>(Controller);
	checkf(Player, TEXT("This is a player only abillity"));

	USpringArmComponent* SpringArm = Cast<USpringArmComponent>(Player->GetAttachedCamera()->GetAttachParent());
	checkf(SpringArm, TEXT("The PlayerCamera must be attached to an USpringArmComponent!"));

	constexpr float RotationScale = 50;
	if (!bIsEnabled)
	{
		SpringArm->CameraLagMaxDistance = SpringArm->CameraRotationLagSpeed * RotationScale;
		SpringArm->CameraRotationLagSpeed = SpringArm->CameraRotationLagSpeed * RotationScale;
	}
	else
	{
		SpringArm->CameraLagMaxDistance = SpringArm->CameraLagMaxDistance / RotationScale;
		SpringArm->CameraRotationLagSpeed = SpringArm->CameraRotationLagSpeed / RotationScale;
	}
	
}
}



UAmenotejikaraAbillity::UAmenotejikaraAbillity()
{
	AmenotejikaraSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AmenotejikaraTargetDetector"));
	AmenotejikaraSphere->InitSphereRadius(AmenotejikaraRadius);
	AmenotejikaraSphere->SetCollisionProfileName("Trigger");
	AmenotejikaraSphere->bShouldCollideWhenPlacing = true;
	AmenotejikaraSphere->OnComponentBeginOverlap.AddDynamic(this, &UAmenotejikaraAbillity::OnActorOverlapWithAmenotejikaraSphere);
	AmenotejikaraSphere->OnComponentEndOverlap.AddDynamic(this, &UAmenotejikaraAbillity::OnActorEndOverlapWithAmenotejikaraSphere);
	AmenotejikaraSphere->CanCharacterStepUpOn = ECB_No;
	AmenotejikaraSphere->SetCollisionObjectType(ECC_WorldDynamic);
	AmenotejikaraSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

#ifdef DEBUG_SHAPES
	AmenotejikaraSphere->SetHiddenInGame(false);
	AmenotejikaraSphere->SetOnlyOwnerSee(true);
#endif
}

void UAmenotejikaraAbillity::OnPosses(AController* Controller)
{
	Super::OnPosses(Controller);

	AmenotejikaraSphere->AttachToComponent(Controller->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

void UAmenotejikaraAbillity::StartExecution(AController* Controller)
{
	Super::StartExecution(Controller);

	UWorld* ThisWorld = GetWorld();
	check(ThisWorld);

	// Slow Down Time
	ThisWorld->GetWorldSettings()->SetTimeDilation(TimeDilationWhenUsed);

	SetCameraRotationLag(Controller, false);
}

void UAmenotejikaraAbillity::FinishExecution(AController* Controller)
{
	ACombatPlayerController* Player = Cast<ACombatPlayerController>(Controller);
	checkf(Player, TEXT("This is a player only abillity"));

	Super::FinishExecution(Controller);

	if (BestToSwapPlaces)
	{
		ACharacter* OwnerCharacter = Controller->GetCharacter();
		UE_LOG(LogPlayerState, Warning, TEXT("Teleporting to %s"), *BestToSwapPlaces->GetFName().ToString());

		// TODO: This relies that both actors have close to same bounding boxes.
		// If the targeted actor is bigger that the owning actor, the targeted actor
		// will fall under the map when the owning actor is on the ground and vice versa.
		check(FMath::Abs(BestToSwapPlaces->GetSimpleCollisionCylinderExtent().Z - OwnerCharacter->GetSimpleCollisionCylinderExtent().Z) < 30.f);

		// Swap Positions, Preserve Rotations and Physics
		FVector OldLocation = BestToSwapPlaces->GetActorLocation();
		BestToSwapPlaces->SetActorLocation(OwnerCharacter->GetActorLocation(), false, nullptr, ETeleportType::TeleportPhysics);
		OwnerCharacter->SetActorLocation(OldLocation, false, nullptr, ETeleportType::TeleportPhysics);
	}

	// Return to the default Time
	GetWorld()->GetWorldSettings()->SetTimeDilation(1.0f);

	SetCameraRotationLag(Controller, true);

	if (BestToSwapPlaces)
	{
		SwapMaterials(nullptr);
		BestToSwapPlaces = nullptr;
		BestToSwapPlacesOriginalMaterial = nullptr;
	}

	LastForwardVector = FVector(FLT_MIN, FLT_MIN, FLT_MIN);
}

void UAmenotejikaraAbillity::Update(AController* Controller)
{
	ACombatPlayerController* Player = Cast<ACombatPlayerController>(Controller);
	checkf(Player, TEXT("This is a player only abillity at the moment"));

	FVector ThisFrameForward = Player->GetAttachedCamera()->GetForwardVector();
	if (ThisFrameForward.Equals(LastForwardVector, 0.01))
	{
		// No need to go down if camera hasn't moved this frame;
		return;
	}

	LastForwardVector = ThisFrameForward;

	UWorld* ThisWorld = Player->GetWorld();
	check(ThisWorld);


	ACharacter* OwnerCharacter = Controller->GetCharacter();
	FVector CharacterBoundsExtent = OwnerCharacter->GetCapsuleComponent()->GetCollisionShape().GetExtent() * FVector(0.5f, 0.5f, 1.f);

	FVector PlayerLocation = OwnerCharacter->GetActorLocation() + CharacterBoundsExtent;

	float MinimumRadiansDifference = FLT_MAX;
	for (AActor* ActorInRange : ActorsInRange)
	{
		FVector ToTarget = ActorInRange->GetActorLocation() - PlayerLocation;

		float DotToTarget = FVector::DotProduct(ToTarget.GetSafeNormal(), ThisFrameForward);
		if (DotToTarget < 0.0f)
		{
			continue;
		}
#ifdef DEBUG_SHAPES
		DrawDebugLine(ThisWorld, PlayerLocation, ActorInRange->GetActorLocation(), FColor::Green, false, 5, 0, 10);
#endif

		float RadiansToTarget = FMath::Acos(DotToTarget);

		//UE_LOG(LogPlayerState, Warning, TEXT("%s : %f"), *ActorInRange->GetFName().ToString(), RadiansToTarget);

		bool bIsInCenterRange = (RadiansToTarget < ToleranceToBeATargetInRadians) & (RadiansToTarget < MinimumRadiansDifference);
		if (!bIsInCenterRange)
		{
			continue;
		}

		// Check if this actor isn't actually behind the already best actor
		if (BestToSwapPlaces)
		{
			FVector BestLocation = BestToSwapPlaces->GetActorLocation();
			FVector NextLocation = ActorInRange->GetActorLocation();

			// TODO: This is incorrect
			float CurrentDistance = FMath::Square(PlayerLocation.X - BestToSwapPlaces->GetActorLocation().X);
			float NextDistance = FMath::Square(PlayerLocation.X - ActorInRange->GetActorLocation().X);

			// float CurrentDistance = FVector::DistSquared(PlayerLocation, BestToSwapPlaces->GetActorLocation());
			// float NextDistance = FVector::DistSquared(PlayerLocation, ActorInRange->GetActorLocation());

			// UE_LOG(LogPlayerState, Warning, TEXT("Curr: %s - %f, Next: %s - %f"),
			//        *BestToSwapPlaces->GetFName().ToString(), CurrentDistance, *ActorInRange->GetFName().ToString(),
			//        NextDistance);
			if (CurrentDistance < NextDistance && BestLocation.Y < NextLocation.Y && BestLocation.Z > NextLocation.Z)
			{
				continue;
			}
		}

		if (BestToSwapPlaces != ActorInRange)
		{
			SwapMaterials(ActorInRange);
			BestToSwapPlaces = ActorInRange;
		}

		MinimumRadiansDifference = RadiansToTarget;
	}
}

bool UAmenotejikaraAbillity::HasFinishedExecution() const
{
	return Super::HasFinishedExecution();
}

void UAmenotejikaraAbillity::BeginDestroy()
{
	// Do not notify for overlaps when the world is being destroyed
	if (IsPendingKill() && AmenotejikaraSphere)
	{
		AmenotejikaraSphere->OnComponentBeginOverlap.RemoveAll(this);
		AmenotejikaraSphere->OnComponentEndOverlap.RemoveAll(this);
		AmenotejikaraSphere->OnComponentBeginOverlap.Clear();
		AmenotejikaraSphere->OnComponentEndOverlap.Clear();
	}

	Super::BeginDestroy();
}

void UAmenotejikaraAbillity::OnActorOverlapWithAmenotejikaraSphere(UPrimitiveComponent*,
                                                                   AActor* Other,
                                                                   UPrimitiveComponent*,
                                                                   int32,
                                                                   bool,
                                                                   const FHitResult&)
{
	ActorsInRange.Add(Other);
}

void UAmenotejikaraAbillity::OnActorEndOverlapWithAmenotejikaraSphere(UPrimitiveComponent*,
                                                                      AActor* OtherActor,
                                                                      UPrimitiveComponent*,
                                                                      int32)
{
	ActorsInRange.RemoveSingleSwap(OtherActor);
}

void UAmenotejikaraAbillity::SwapMaterials(AActor* WithActor)
{
	if (BestToSwapPlaces)
	{
		check(BestToSwapPlacesOriginalMaterial);

		UMeshComponent* CurrentBestPrimitive = Cast<UMeshComponent>(BestToSwapPlaces->GetComponentByClass(UMeshComponent::StaticClass()));
		check(CurrentBestPrimitive);
		CurrentBestPrimitive->SetMaterial(0, BestToSwapPlacesOriginalMaterial);
	}

	if (WithActor)
	{
		UMeshComponent* NextBestPrimitive = Cast<UMeshComponent>(WithActor->GetComponentByClass(UMeshComponent::StaticClass()));
		check(NextBestPrimitive);

		BestToSwapPlacesOriginalMaterial = NextBestPrimitive->GetMaterial(0);
		check(BestToSwapPlacesOriginalMaterial != MarkAsSelectedMaterial);
		NextBestPrimitive->SetMaterial(0, MarkAsSelectedMaterial);
	}
}
