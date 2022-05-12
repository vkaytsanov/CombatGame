// Fill out your copyright notice in the Description page of Project Settings.

#include "MeleeAIController.h"

#include "CharacterBase.h"
#include "MeleeCharacter.h"
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
	//SetActorEnableCollision(false);

	HearingSphere = CreateDefaultSubobject<USphereComponent>(TEXT("HearingSphere"));
	HearingSphere->CanCharacterStepUpOn = ECB_No;
	HearingSphere->bDrawOnlyIfSelected = false;
	HearingSphere->ShapeColor = FColor::Turquoise;
	HearingSphere->bShouldCollideWhenPlacing = true;
	HearingSphere->SetCollisionProfileName("TriggerPawn");
	HearingSphere->SetCollisionObjectType(ECC_WorldDynamic);
	HearingSphere->OnComponentBeginOverlap.AddDynamic(this, &AMeleeAIController::OnHearingOverlap);
	HearingSphere->OnComponentEndOverlap.AddDynamic(this, &AMeleeAIController::OnSensesEndOverlap);

	SightSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SightSphere"));
	SightSphere->bDrawOnlyIfSelected = false;
	SightSphere->ShapeColor = FColor::Emerald;
	SightSphere->bShouldCollideWhenPlacing = true;
	SightSphere->CanCharacterStepUpOn = ECB_No;
	SightSphere->SetCollisionProfileName("TriggerPawn");
	SightSphere->SetCollisionObjectType(ECC_WorldDynamic);
	SightSphere->OnComponentBeginOverlap.AddDynamic(this, &AMeleeAIController::OnSightOverlap);
	SightSphere->OnComponentEndOverlap.AddDynamic(this, &AMeleeAIController::OnSensesEndOverlap);

#if DEBUG_SHAPES
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

	CharacterBase = Cast<ACharacterBase>(InPawn);

	AttachToActor(InPawn, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	HearingSphere->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	SightSphere->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	UCharacterMovementComponent* MovementComponent = CharacterBase->GetCharacterMovement();
	SpawnMaxSpeed = MovementComponent->MaxWalkSpeed;
	SpawnLocation = CharacterBase->GetActorLocation();

	SetCurrentState(CES_IdleAction);
	CurrentEnemyState->Enter(this);
}

void AMeleeAIController::OnFatalDamageTaken()
{
	UE_LOG(LogMeleeAIController, Warning, TEXT("%s is Dead"), *CharacterBase->GetFName().ToString());

	// TODO:
	// Drop Item
	// ..

	// Give player exp
	// ..

	CharacterBase->Destroy();
}

void AMeleeAIController::SetCurrentState(ECurrentCombatEnemyState State)
{
#define ENUM_TO_STRUCT_STATE(State)                  \
	case CES_##State##Action:                        \
		CurrentEnemyState = &State##EnemyState;      \
		return;

	switch (State)
	{
		COMBAT_ENEMY_STATE(ENUM_TO_STRUCT_STATE);
	}

	checkf(false, TEXT("This shouldn't be reached"));
#undef ENUM_TO_STRUCT_STATE
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
	FCombatEnemyState* ThisFrameEnemyState = CurrentEnemyState;
	CurrentEnemyState->Update(this);
	if (ThisFrameEnemyState != CurrentEnemyState)
	{
		CurrentEnemyState->Enter(this);
	}

	Super::Tick(DeltaSeconds);
}

void AMeleeAIController::OnHearingOverlap(UPrimitiveComponent*, AActor* Other, UPrimitiveComponent*, int32, bool, const FHitResult&)
{
	ACharacterBase* PotentialTarget = GetPlayerAsCharacterIfValid(CharacterBase, Other);
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
	ACharacterBase* PotentialTarget = GetPlayerAsCharacterIfValid(CharacterBase, Other);
	if (!PotentialTarget)
	{
		return;
	}

	FVector ToTarget = Other->GetActorLocation() - CharacterBase->GetActorLocation();
	FVector Forward = CharacterBase->GetActorForwardVector();

	float DotToTarget = FVector::DotProduct(ToTarget.GetSafeNormal(), Forward);
	if (DotToTarget < 0.0f)
	{
		return;
	}
	float AngleToTarget = FMath::RadiansToDegrees(FMath::Acos(DotToTarget));

	if (AngleToTarget < SightAngle)
	{
		UE_LOG(LogMeleeAIController, Warning, TEXT("Saw character %s"), *PotentialTarget->GetFName().ToString());
		SetTarget(PotentialTarget);
	}
}

void AMeleeAIController::OnSensesEndOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int32)
{
	ACharacterBase* PotentialTarget = GetPlayerAsCharacterIfValid(CharacterBase, OtherActor);
	if (PotentialTarget && CurrentTarget == PotentialTarget)
	{
		CurrentTarget = nullptr;
		SetCurrentState(CES_ReturnHomeAction);
	}
}

void AMeleeAIController::SetTarget(ACharacterBase* Target)
{
	check(Target);

	ChaseStartLocation = CharacterBase->GetNavAgentLocation();
	CurrentTarget = Target;

	SetCurrentState(CES_ChaseAction);
}
