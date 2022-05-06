// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatPlayerController.h"

#include "DrawDebugHelpers.h"
#include "MeleeCharacter.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogCombatPC, Warning, All);

ACombatPlayerController::ACombatPlayerController()
{
	InputComponent = CreateDefaultSubobject<UInputComponent>(TEXT("InputComponent"));

	AmenotejikaraSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AmenotejikaraTargetDetector"));
	AmenotejikaraSphere->InitSphereRadius(AmenotejikaraRadius);
	AmenotejikaraSphere->SetCollisionProfileName("Trigger");
	AmenotejikaraSphere->OnComponentBeginOverlap.AddDynamic(this, &ACombatPlayerController::OnActorOverlapWithAmenotejikaraSphere);
	AmenotejikaraSphere->OnComponentEndOverlap.AddDynamic(this, &ACombatPlayerController::OnActorEndOverlapWithAmenotejikaraSphere);
	AmenotejikaraSphere->CanCharacterStepUpOn = ECB_No;

#if WITH_EDITORONLY_DATA
	AActor::SetActorHiddenInGame(false);
	AmenotejikaraSphere->SetHiddenInGame(false);
	AmenotejikaraSphere->SetOnlyOwnerSee(true);
#endif
}

void ACombatPlayerController::OnFatalDamageTaken()
{
	
}

void ACombatPlayerController::SetCurrentState(ECurrentCombatPlayerState State)
{
#define ENUM_TO_STRUCT_STATE(State)                  \
	case CPS_##State##Action:                        \
		CurrentPlayerState = &State##PlayerState;    \
		return;

	switch (State)
	{
		COMBAT_PLAYER_STATES(ENUM_TO_STRUCT_STATE);
	}

	checkf(false, TEXT("This shouldn't be reached"));
#undef ENUM_TO_STRUCT_STATE
}

void ACombatPlayerController::BeginDestroy()
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

void ACombatPlayerController::OnPossess(APawn* InPawn)
{
	if (!InPawn || InPawn->IsPendingKill())
	{
		return;
	}

	Super::OnPossess(InPawn);

	AttachToActor(InPawn, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	AmenotejikaraSphere->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

void ACombatPlayerController::BeginPlay()
{
	Super::BeginPlay();

	APawn* OwnerPawn = GetPawn();
	PlayerCamera = Cast<UCameraComponent>(OwnerPawn->GetComponentByClass(UCameraComponent::StaticClass()));
	checkf(PlayerCamera, TEXT("The player class must have a UCameraComponent!"));

	USceneComponent* SpringArmPrototype = PlayerCamera->GetAttachParent();
	checkf(SpringArmPrototype, TEXT("The PlayerCamera must be attached to an USpringArmComponent!"));

	USpringArmComponent* SpringArm = Cast<USpringArmComponent>(SpringArmPrototype);
	checkf(SpringArm, TEXT("The PlayerCamera must be attached to an USpringArmComponent!"));

	SpringArm->bEnableCameraLag = true;
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraLagMaxDistance = 10.0f;
	SpringArm->CameraLagMaxTimeStep = 2.0f;
	SpringArm->CameraLagSpeed = 3;
	SpringArm->CameraRotationLagSpeed = 5;
	SpringArm->bUsePawnControlRotation = true;

	AttachToActor(OwnerPawn, FAttachmentTransformRules::KeepRelativeTransform);
}

void ACombatPlayerController::SetupInputComponent()
{
	check(InputComponent);

	InputComponent->BindAxis("MoveForward");
	InputComponent->BindAxis("MoveRight");

	InputComponent->BindAxis("Turn", this, &APlayerController::AddYawInput);
	InputComponent->BindAxis("TurnRate", this, &ACombatPlayerController::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APlayerController::AddPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &ACombatPlayerController::LookUpAtRate);

	Super::SetupInputComponent();
	SetCurrentState(CPS_IdleAction);
}

void ACombatPlayerController::Tick(float DeltaTime)
{
	FCombatPlayerState* ThisFramePlayerState = CurrentPlayerState;
	CurrentPlayerState->Update(this);
	if (ThisFramePlayerState != CurrentPlayerState)
	{
		CurrentPlayerState->Enter(this);
	}

	Super::Tick(DeltaTime);

}


void ACombatPlayerController::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		GetPawn()->AddMovementInput(GetPawn()->GetActorForwardVector(), Value);
	}
}

void ACombatPlayerController::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		GetPawn()->AddMovementInput(GetPawn()->GetActorRightVector(), Value);
	}
}

void ACombatPlayerController::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddYawInput(Rate * 45.f * GetWorld()->GetDeltaSeconds());
}

void ACombatPlayerController::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddPitchInput(Rate * 45.f * GetWorld()->GetDeltaSeconds());
}

void ACombatPlayerController::OnActorOverlapWithAmenotejikaraSphere(UPrimitiveComponent*,
                                                                    AActor* Other,
                                                                    UPrimitiveComponent*,
                                                                    int32,
                                                                    bool,
                                                                    const FHitResult&)
{
	AmenotejikaraPlayerState.AddActorInRange(Other);
}

void ACombatPlayerController::OnActorEndOverlapWithAmenotejikaraSphere(UPrimitiveComponent* OverlappedComponent,
                                                                       AActor* OtherActor,
                                                                       UPrimitiveComponent* OtherComp,
                                                                       int32 OtherBodyIndex)
{
	AmenotejikaraPlayerState.RemoveActorOutOfRange(OtherActor);
}

#undef COMBAT_PLAYER_STATES
