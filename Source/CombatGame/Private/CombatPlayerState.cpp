#include "CombatPlayerState.h"

#include "CharacterBase.h"
#include "CombatPlayerController.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

void FIdleActionState::Enter(ACombatPlayerController* Player)
{
}

void FIdleActionState::Update(ACombatPlayerController* Player)
{
	if (Player->GetInputKeyTimeDown(EKeys::SpaceBar))
	{
		Player->SetCurrentState(CPS_JumpAction);
	}
	else if (Player->GetInputKeyTimeDown(EKeys::LeftMouseButton))
	{
		Player->SetCurrentState(CPS_AttackAction);
	}
	else if (Player->GetInputKeyTimeDown(EKeys::One))
	{
		Player->SetCurrentState(CPS_AmenotejikaraAction);
	}

	Player->MoveForward(Player->GetInputAxisValue(FName("MoveForward")));
	Player->MoveRight(Player->GetInputAxisValue(FName("MoveRight")));
}

void FJumpActionState::Enter(ACombatPlayerController* Player)
{
	ACharacter* OwnerCharacter = Player->GetCharacter();
	OwnerCharacter->Jump();
}

void FJumpActionState::Update(ACombatPlayerController* Player)
{
	ACharacter* OwnerCharacter = Player->GetCharacter();

	if (!OwnerCharacter->bWasJumping)
	{
		Player->SetCurrentState(CPS_IdleAction);
	}
}

void FAttackActionState::Enter(ACombatPlayerController* Player) {}

void FAttackActionState::Update(ACombatPlayerController* Player)
{
	// TODO: Temp
	Player->SetCurrentState(CPS_IdleAction);
}

void FAmenotejikaraActionState::Enter(ACombatPlayerController* Player)
{
	UWorld* ThisWorld = Player->GetWorld();
	check(ThisWorld);

	// AudioTime is NOT Dilated and is paused when the game is Paused
	EnterStateTime = ThisWorld->GetAudioTimeSeconds();

	// Slow Down Time
	ThisWorld->GetWorldSettings()->SetTimeDilation(0.05f);
}

void FAmenotejikaraActionState::Update(ACombatPlayerController* Player)
{
	UWorld* ThisWorld = Player->GetWorld();
	check(ThisWorld);

	// Either when the time runs out or the key was released prematurely
	bool bShouldLeaveState = EnterStateTime + MaxStateTime < ThisWorld->GetAudioTimeSeconds();
	bShouldLeaveState |= !Player->GetInputKeyTimeDown(EKeys::One);

	if (bShouldLeaveState)
	{
		ACharacter* OwnerCharacter = Player->GetCharacter();
		FVector CharacterBounds = OwnerCharacter->GetCapsuleComponent()->GetCollisionShape().GetExtent() * FVector(0.5f, 0.5f, 1.f);
		FVector RayStart = OwnerCharacter->GetActorLocation() + CharacterBounds;
		FVector RayEnd = RayStart + Player->GetAttachedCamera()->GetForwardVector() * 800;
		
#if WITH_EDITORONLY_DATA
		DrawDebugLine(ThisWorld, RayStart, RayEnd, FColor::Green, false, 5, 0, 10);
#endif


		// Return to the default Time
		ThisWorld->GetWorldSettings()->SetTimeDilation(1.0f);

		// Shoot Ray
		FHitResult Result;
		ThisWorld->LineTraceSingleByChannel(Result, RayStart, RayEnd, ECC_GameTraceChannel1);

		AActor* HitActor = Result.GetActor();
		if (HitActor)
		{
			FVector HitActorLocation = HitActor->GetActorLocation();

			// Swap Positions, Preserve Rotations
			if (HitActor->IsA<ACharacterBase>())
			{
				HitActor->SetActorLocation(OwnerCharacter->GetActorLocation(), false, nullptr, ETeleportType::TeleportPhysics);
				OwnerCharacter->SetActorLocation(HitActorLocation, false, nullptr, ETeleportType::TeleportPhysics);
			}
		}

		Player->SetCurrentState(CPS_IdleAction);
	}
}
