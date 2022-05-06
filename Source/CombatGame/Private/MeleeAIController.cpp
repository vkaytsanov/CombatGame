// Fill out your copyright notice in the Description page of Project Settings.

#include "MeleeAIController.h"

#include "CharacterBase.h"
#include "NavigationSystem.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"


DEFINE_LOG_CATEGORY_STATIC(LogMeleeAIController, Warning, All);

namespace
{
ACharacterBase* GetPlayerAsCharacterIfValid(APawn* ThisPawn, AActor* OverlappedActor)
{
	ACharacterBase* PotentialTarget = Cast<ACharacterBase>(OverlappedActor);
	if ((!ThisPawn | !PotentialTarget) || ThisPawn == OverlappedActor)
	{
		return nullptr;
	}

	AController* OtherController = PotentialTarget->GetController();
	if (!OtherController || !OtherController->IsA<APlayerController>())
	{
		return nullptr;
	}

	return PotentialTarget;
}

}

AMeleeAIController::AMeleeAIController()
{
	bStartAILogicOnPossess = true;
	bStopAILogicOnUnposses = true;
	bAttachToPawn = true;
	bSetControlRotationFromPawnOrientation = true;

	// Disable collision for the CONTROLLER
	SetActorEnableCollision(false);

	HearingSphere = CreateDefaultSubobject<USphereComponent>(TEXT("HearingSphere"));
	HearingSphere->CanCharacterStepUpOn = ECB_No;
	HearingSphere->bDrawOnlyIfSelected = false;
	HearingSphere->ShapeColor = FColor::Turquoise;
	HearingSphere->bShouldCollideWhenPlacing = true;
	HearingSphere->SetCollisionObjectType(ECC_Pawn);
	HearingSphere->SetCollisionProfileName("Trigger");
	HearingSphere->OnComponentBeginOverlap.AddDynamic(this, &AMeleeAIController::OnHearingOverlap);
	HearingSphere->OnComponentEndOverlap.AddDynamic(this, &AMeleeAIController::OnSensesEndOverlap);

	SightSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SightSphere"));
	SightSphere->bDrawOnlyIfSelected = false;
	SightSphere->ShapeColor = FColor::Emerald;
	SightSphere->bShouldCollideWhenPlacing = true;
	SightSphere->CanCharacterStepUpOn = ECB_No;
	SightSphere->SetCollisionObjectType(ECC_Pawn);
	SightSphere->SetCollisionProfileName("Trigger");
	SightSphere->OnComponentBeginOverlap.AddDynamic(this, &AMeleeAIController::OnSightOverlap);
	SightSphere->OnComponentEndOverlap.AddDynamic(this, &AMeleeAIController::OnSensesEndOverlap);


	SetReturningHome();

#if WITH_EDITORONLY_DATA
	AActor::SetHidden(false);
	HearingSphere->bHiddenInGame = false;
	SightSphere->bHiddenInGame = false;
#endif
}

void AMeleeAIController::OnPossess(APawn* InPawn)
{
	if (!InPawn || InPawn->IsPendingKill())
	{
		return;
	}

	Super::OnPossess(InPawn);

	AttachToActor(InPawn, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	HearingSphere->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	SightSphere->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

void AMeleeAIController::SetReturningHome()
{
	HearingSphere->SetSphereRadius(0.0f);
	SightSphere->SetSphereRadius(0.0f);

	APawn* OwnerPawn = GetPawn();
	if (OwnerPawn)
	{
		EPathFollowingRequestResult::Type RequestResult = MoveToLocation(ChaseStartLocation);
		if (RequestResult == EPathFollowingRequestResult::RequestSuccessful)
		{
			UE_LOG(LogMeleeAIController, Warning, TEXT("%s is going back home at %s"), *OwnerPawn->GetFName().ToString(), *ChaseStartLocation.ToString());
		}
	}

	CurrentTarget = nullptr;
}

void AMeleeAIController::OnReturnedHome()
{
	HearingSphere->SetSphereRadius(HearingRadius);
	SightSphere->SetSphereRadius(SightRadius);
}

void AMeleeAIController::BeginPlay()
{
	Super::BeginPlay();

	ACharacter* OwnerCharacter = GetCharacter();
	UCharacterMovementComponent* MovementComponent = OwnerCharacter->GetCharacterMovement();
	SpawnMaxSpeed = MovementComponent->MaxWalkSpeed;
	SpawnLocation = OwnerCharacter->GetActorLocation();
}

void AMeleeAIController::BeginDestroy()
{
	// Do not notify for overlaps when the world is being destroyed
	if (IsPendingKill() && SightSphere)
	{
		check(HearingSphere);

		SightSphere->OnComponentBeginOverlap.RemoveAll(this);
		SightSphere->OnComponentEndOverlap.RemoveAll(this);
		SightSphere->OnComponentBeginOverlap.Clear();
		SightSphere->OnComponentEndOverlap.Clear();

		HearingSphere->OnComponentBeginOverlap.RemoveAll(this);
		HearingSphere->OnComponentEndOverlap.RemoveAll(this);
		HearingSphere->OnComponentBeginOverlap.Clear();
		HearingSphere->OnComponentEndOverlap.Clear();
	}

	Super::BeginDestroy();
}

void AMeleeAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (CurrentTarget)
	{
		CurrentTarget->GetCharacterMovement()->MaxWalkSpeed = SpawnMaxSpeed;
		MoveToActor(CurrentTarget);
		return;
	}

	if (GetPathFollowingComponent()->GetStatus() != EPathFollowingStatus::Idle)
	{
		return;
	}

	Wander();
}

void AMeleeAIController::OnHearingOverlap(UPrimitiveComponent*, AActor* Other, UPrimitiveComponent*, int32, bool, const FHitResult&)
{
	ACharacterBase* PotentialTarget = GetPlayerAsCharacterIfValid(GetPawn(), Other);
	if (!PotentialTarget)
	{
		return;
	}

	// Stealth logic if any ..

	UE_LOG(LogMeleeAIController, Warning, TEXT("Heard character %s"), *PotentialTarget->GetFName().ToString());
	SetTarget(PotentialTarget);
}


void AMeleeAIController::OnSightOverlap(UPrimitiveComponent*, AActor* Other, UPrimitiveComponent*, int32, bool, const FHitResult&)
{
	ACharacter* OwnerCharacter = GetCharacter();

	ACharacterBase* PotentialTarget = GetPlayerAsCharacterIfValid(OwnerCharacter, Other);
	if (!PotentialTarget)
	{
		return;
	}

	FVector ToTarget = Other->GetActorLocation() - OwnerCharacter->GetActorLocation();
	FVector Forward = GetPawn()->GetActorForwardVector();

	if (FVector::DotProduct(ToTarget, Forward) < 0.0f)
	{
		return;
	}

	float DotToTarget = FVector::DotProduct(ToTarget.GetSafeNormal(), Forward.GetSafeNormal());
	float AngleToTarget = FMath::RadiansToDegrees(FMath::Acos(DotToTarget));

	if (AngleToTarget < SightAngle)
	{
		SetTarget(PotentialTarget);
	}
}

void AMeleeAIController::OnSensesEndOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int32)
{
	ACharacterBase* PotentialTarget = GetPlayerAsCharacterIfValid(GetPawn(), OtherActor);
	if (PotentialTarget)
	{
		SetReturningHome();
	}
}

void AMeleeAIController::SetTarget(ACharacterBase* Target)
{
	check(Target);

	ChaseStartLocation = GetPawn()->GetNavAgentLocation();
	CurrentTarget = Target;
}

void AMeleeAIController::Wander()
{
	UNavigationSystemV1* NavSystem = Cast<UNavigationSystemV1>(GetWorld()->GetNavigationSystem());
	check(NavSystem);

	FNavLocation NewLocation;
	if (!NavSystem->GetRandomPointInNavigableRadius(SpawnLocation, WanderRadius, NewLocation))
	{
		return;
	}

	UCharacterMovementComponent* MovementComponent = GetCharacter()->GetCharacterMovement();
	MovementComponent->MaxWalkSpeed = 150.f;

	EPathFollowingRequestResult::Type RequestResult = MoveToLocation(NewLocation.Location);

	if (RequestResult == EPathFollowingRequestResult::RequestSuccessful)
	{
		// If the wander happens after returning back
		if (HearingSphere->IsZeroExtent())
		{
			OnReturnedHome();
		}
	}
}
