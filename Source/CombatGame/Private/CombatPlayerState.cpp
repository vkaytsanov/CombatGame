#include "CombatPlayerState.h"

#include "CharacterBase.h"
#include "CombatPlayerController.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

DEFINE_LOG_CATEGORY_STATIC(LogPlayerState, Warning, All);

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
	ThisWorld->GetWorldSettings()->SetTimeDilation(TimeDilationWhenUsed);
}

void FAmenotejikaraActionState::Update(ACombatPlayerController* Player)
{
	UWorld* ThisWorld = Player->GetWorld();
	check(ThisWorld);

	// Either when the time runs out or the key was released prematurely
	bool bShouldLeaveState = EnterStateTime + MaxTimeInThisStateInSeconds < ThisWorld->GetAudioTimeSeconds();
	bShouldLeaveState |= !Player->GetInputKeyTimeDown(EKeys::One);

	if (bShouldLeaveState)
	{
		UE_LOG(LogPlayerState, Warning, TEXT("%d"), ActorsInRange.Num());

		ACharacter* OwnerCharacter = Player->GetCharacter();
		FVector CharacterBoundsExtent = OwnerCharacter->GetCapsuleComponent()->GetCollisionShape().GetExtent() * FVector(0.5f, 0.5f, 1.f);

		FVector PlayerLocation = OwnerCharacter->GetActorLocation() + CharacterBoundsExtent;
		FVector PlayerForward = (Player->GetAttachedCamera()->GetForwardVector() + OwnerCharacter->GetActorForwardVector()).GetUnsafeNormal();

		AActor* BestToSwapPlaces = nullptr;
		float MinimumRadiansDifference = FLT_MAX;
		for (AActor* ActorInRange : ActorsInRange)
		{
			FVector ToTarget = ActorInRange->GetActorLocation() - PlayerLocation;

			bool bIsFacingTarget = FVector::DotProduct(ToTarget, PlayerForward) >= 0.0f;
			if (!bIsFacingTarget)
			{
				continue;
			}

#if WITH_EDITORONLY_DATA
			DrawDebugLine(ThisWorld, PlayerLocation, ActorInRange->GetActorLocation(), FColor::Green, false, 5, 0, 10);
#endif
			float DotToTarget = FVector::DotProduct(ToTarget.GetSafeNormal(), PlayerForward.GetSafeNormal());
			float RadiansToTarget = FMath::Acos(DotToTarget);

			UE_LOG(LogPlayerState, Warning, TEXT("%s : %f"), *ActorInRange->GetFName().ToString(), RadiansToTarget);

			if (RadiansToTarget > ToleranceToBeATargetInRadians)
			{
				continue;
			}

			if (RadiansToTarget >= MinimumRadiansDifference)
			{
				continue;
			}

			// Check if this actor isn't actually behind the already best actor
			if (BestToSwapPlaces)
			{
				float CurrentDistance = FVector::DistSquared(PlayerLocation, BestToSwapPlaces->GetActorLocation());
				float NextDistance = FVector::DistSquared(PlayerLocation, ActorInRange->GetActorLocation());

				UE_LOG(LogPlayerState, Warning, TEXT("Curr: %s - %f, Next: %s - %f"), *BestToSwapPlaces->GetFName().ToString(), CurrentDistance, *ActorInRange->GetFName().ToString(), NextDistance);
				if (CurrentDistance < NextDistance)
				{
					continue;
				}
			}

			BestToSwapPlaces = ActorInRange;
			MinimumRadiansDifference = RadiansToTarget;

		}

		if (BestToSwapPlaces)
		{
			UE_LOG(LogPlayerState, Warning, TEXT("Teleporting to %s"), *BestToSwapPlaces->GetFName().ToString());

			FVector NewLocation = BestToSwapPlaces->GetActorLocation();

			// Swap Positions, Preserve Rotations and Physics
			BestToSwapPlaces->SetActorLocation(OwnerCharacter->GetActorLocation(), false, nullptr, ETeleportType::TeleportPhysics);
			OwnerCharacter->SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
		}

		// Return to the default Time
		ThisWorld->GetWorldSettings()->SetTimeDilation(1.0f);
		Player->SetCurrentState(CPS_IdleAction);
	}
}
