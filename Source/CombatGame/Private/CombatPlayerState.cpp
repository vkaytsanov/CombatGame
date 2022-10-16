#include "CombatPlayerState.h"

#include "CharacterBase.h"
#include "CombatPlayerController.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Abillities/CharacterAbillity.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

DEFINE_LOG_CATEGORY_STATIC(LogPlayerState, Warning, All);

void FIdlePlayerState::Enter(ACombatPlayerController* Player)
{
}

void FIdlePlayerState::Update(ACombatPlayerController* Player)
{
	if (Player->IsInputKeyDown(EKeys::SpaceBar))
	{
		Player->SetCurrentState(CPS_JumpAction);
	}
	else if (Player->IsInputKeyDown(EKeys::LeftMouseButton))
	{
		ACharacterBase* OwnerPlayer = Player->GetCharacterBase();
		if (OwnerPlayer->CanAttack())
		{
			Player->SetCurrentState(CPS_AttackAction);
		}
	}
	else
	{
		const TArray<UCharacterAbillity*>& Abillities = Player->GetCharacterBase()->GetAbillities();
		for (UCharacterAbillity* Abillity : Abillities)
		{
			if (Abillity->IsExecutable() & Player->IsInputKeyDown(Abillity->ActivationKey))
			{
				Player->SetCurrentRunningAbillity(Abillity);
				Player->SetCurrentState(CPS_UsingAbillityAction);
				break;
			}
		}
	}

	Player->UpdateMovement();
}

void FJumpPlayerState::Enter(ACombatPlayerController* Player)
{
	ACharacter* OwnerCharacter = Player->GetCharacter();
	OwnerCharacter->Jump();
}

void FJumpPlayerState::Update(ACombatPlayerController* Player)
{
	ACharacter* OwnerCharacter = Player->GetCharacter();
	if (!OwnerCharacter->bWasJumping)
	{
		Player->SetCurrentState(CPS_IdleAction);
	}
}

void FAttackPlayerState::Enter(ACombatPlayerController* Player)
{
	Player->GetCharacterBase()->OnAttackStart_BP();
}

void FAttackPlayerState::Update(ACombatPlayerController* Player)
{
	if (Player->WasInputKeyJustPressed(EKeys::LeftMouseButton))
	{
		bCanAdvanceAttackAfterNotifyForHit = true;
	}
}

void FUsingAbillityPlayerState::Enter(ACombatPlayerController* Player)
{
	UCharacterAbillity* CurrentAbillity = Player->GetCurrentRunningAbillity();
	check(CurrentAbillity);

	CurrentAbillity->StartExecution(Player);
}

void FUsingAbillityPlayerState::Update(ACombatPlayerController* Player)
{
	UCharacterAbillity* CurrentAbillity = Player->GetCurrentRunningAbillity();
	check(CurrentAbillity);

	if (CurrentAbillity->bCanMoveWhenExecuting)
	{
		Player->UpdateMovement();
	}

	if (CurrentAbillity->bCanAttackWhenExecuting)
	{
		if (Player->IsInputKeyDown(EKeys::LeftMouseButton))
		{
			ACharacterBase* OwnerPlayer = Player->GetCharacterBase();
			if (OwnerPlayer->CanAttack())
			{
				Player->SetCurrentState(CPS_AttackAction);
			}
		}
	}

	if (CurrentAbillity->bCanJumpWhenExecuting)
	{
		if (Player->IsInputKeyDown(EKeys::SpaceBar))
		{
			Player->GetCharacterBase()->Jump();
		}
	}

	bool bIsUserStopped = CurrentAbillity->IsTargetable() & !Player->IsInputKeyDown(CurrentAbillity->ActivationKey);
	if (CurrentAbillity->HasFinishedExecution() | bIsUserStopped)
	{
		CurrentAbillity->FinishExecution(Player);
		Player->SetCurrentRunningAbillity(nullptr);
		Player->SetCurrentState(CPS_IdleAction);
	}
	else
	{
		CurrentAbillity->Update(Player);
	}
}
