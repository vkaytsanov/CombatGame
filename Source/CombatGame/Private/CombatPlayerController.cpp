// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatPlayerController.h"

#include "DrawDebugHelpers.h"
#include "MeleeCharacter.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Abillities/CharacterAbillity.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogCombatPC, Warning, All);

ACombatPlayerController::ACombatPlayerController()
{
	InputComponent = CreateDefaultSubobject<UInputComponent>(TEXT("InputComponent"));
}

void ACombatPlayerController::OnFatalDamageTaken()
{
	// TODO;
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

void ACombatPlayerController::UpdateMovement()
{
	static FName MoveForward("MoveForward");
	if (float Value = this->GetInputAxisValue(MoveForward))
	{
		OwnerPlayer->AddMovementInput(OwnerPlayer->GetActorForwardVector(), Value);
	}

	static FName MoveRight("MoveRight");
	if (float Value = this->GetInputAxisValue(MoveRight))
	{
		OwnerPlayer->AddMovementInput(OwnerPlayer->GetActorRightVector(), Value);
	}
}

bool ACombatPlayerController::IsAttacking() const
{
	return CurrentPlayerState == &AttackPlayerState;
}


bool ACombatPlayerController::ConditionalAdvanceAttack()
{
	check(IsAttacking());

	if (AttackPlayerState.bCanAdvanceAttackAfterNotifyForHit)
	{
		AttackPlayerState.bCanAdvanceAttackAfterNotifyForHit = false;
		OwnerPlayer->ExecuteAttack();
		return true;
	}
	return false;
}

void ACombatPlayerController::OnAttackEnd_BP()
{
	AttackPlayerState.bCanAdvanceAttackAfterNotifyForHit = false;
	SetCurrentState(CPS_IdleAction);
}

void ACombatPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	OwnerPlayer = Cast<ACharacterBase>(InPawn);
	checkf(OwnerPlayer, TEXT("The player class must extend ACharacterBase!"));

	PlayerCamera = Cast<UCameraComponent>(OwnerPlayer->GetComponentByClass(UCameraComponent::StaticClass()));
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
	SpringArm->bDoCollisionTest = false;

	// Will use the controller's rotation for movement
	OwnerPlayer->bUseControllerRotationPitch = false;
	OwnerPlayer->bUseControllerRotationRoll = false;
	OwnerPlayer->bUseControllerRotationYaw = true;
	OwnerPlayer->GetCharacterMovement()->bUseControllerDesiredRotation = false;
	OwnerPlayer->GetCharacterMovement()->bOrientRotationToMovement = false;
	OwnerPlayer->GetCharacterMovement()->RotationRate = FRotator(0, 540, 0);

	AttachToActor(InPawn, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	for (UCharacterAbillity* Abillity : OwnerPlayer->GetAbillities())
	{
		Abillity->OnPosses(this);
	}
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

#undef COMBAT_PLAYER_STATES
