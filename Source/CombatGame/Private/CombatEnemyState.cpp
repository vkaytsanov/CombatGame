#include "CombatEnemyState.h"

#include "CharacterBase.h"
#include "MeleeAIController.h"
#include "NavigationSystem.h"
#include "Abillities/CharacterAbillity.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


void FIdleEnemyState::Enter(AMeleeAIController* Enemy)
{
	IdleTime = Enemy->GetWorld()->GetTimeSeconds();
	Enemy->HearingSphere->SetSphereRadius(Enemy->HearingRadius);
	Enemy->SightSphere->SetSphereRadius(Enemy->SightRadius);
}

void FIdleEnemyState::Update(AMeleeAIController* Enemy)
{
	if (IdleTime + Enemy->WanderDelaySeconds < Enemy->GetWorld()->GetTimeSeconds())
	{
		Enemy->SetCurrentState(CES_WanderAction);
	}
}

void FWanderEnemyState::Enter(AMeleeAIController* Enemy)
{
	ACharacterBase* Character = Enemy->GetCharacterBase();
	UNavigationSystemV1* NavSystem = Cast<UNavigationSystemV1>(Character->GetWorld()->GetNavigationSystem());
	check(NavSystem);

	FNavLocation NewLocation;
	if (!NavSystem->GetRandomPointInNavigableRadius(Character->GetActorLocation(), Enemy->WanderRadius, NewLocation))
	{
		return;
	}

	UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement();
	MovementComponent->MaxWalkSpeed = Enemy->WanderMovementSpeed;

	Enemy->MoveToLocation(NewLocation.Location);
}

void FWanderEnemyState::Update(AMeleeAIController* Enemy)
{
	if (Enemy->GetPathFollowingComponent()->GetStatus() == EPathFollowingStatus::Idle)
	{
		Enemy->SetCurrentState(CES_IdleAction);
	}
}

void FChaseEnemyState::Enter(AMeleeAIController* Enemy)
{
	Enemy->GetCharacterBase()->GetCharacterMovement()->MaxWalkSpeed = 450.f;
}

void FChaseEnemyState::Update(AMeleeAIController* Enemy)
{
	ACharacterBase* Character = Enemy->GetCharacterBase();

	Enemy->MoveToActor(Enemy->GetCurrentTarget(), Enemy->GetCharacterBase()->GetAttackRange());

	const TArray<UCharacterAbillity*>& Abillities = Character->GetAbillities();
	if (Abillities.Num() && Abillities[0]->IsExecutable())
	{
		Enemy->SetCurrentRunningAbillity(Abillities[0]);
		Enemy->SetCurrentState(CES_UsingAbillityAction);
	}
	else if (Character->HasAnyCharactersToAttack())
	{
		Enemy->SetCurrentState(CES_AttackAction);
	}

}

void FAttackEnemyState::Enter(AMeleeAIController* Enemy)
{
	Enemy->StopMovement();
	Enemy->GetCharacterBase()->OnAttackStart_BP();
}

void FAttackEnemyState::Update(AMeleeAIController* Enemy)
{
	ACharacterBase* Character = Enemy->GetCharacterBase();
	

	if (!Character->HasAnyCharactersToAttack())
	{
		float DistanceFromChase = FVector::DistSquared(Character->GetActorLocation(), Enemy->GetChaseStartLocation());
		if (DistanceFromChase > FMath::Square(Enemy->MaxChaseDistance))
		{
			Enemy->SetCurrentState(CES_ReturnHomeAction);
		}
		else
		{
			Enemy->SetCurrentState(CES_ChaseAction);
		}
	}
	else if (Character->CanAttack())
	{
		Character->ExecuteAttack();
	}
}

void FReturnHomeEnemyState::Enter(AMeleeAIController* Enemy)
{
	Enemy->HearingSphere->SetSphereRadius(0.0f);
	Enemy->SightSphere->SetSphereRadius(0.0f);
	Enemy->MoveToLocation(Enemy->GetChaseStartLocation());
}

void FReturnHomeEnemyState::Update(AMeleeAIController* Enemy)
{
	bool bHasReturnedHome = Enemy->GetPathFollowingComponent()->DidMoveReachGoal();
	if (bHasReturnedHome)
	{
		Enemy->SetCurrentState(CES_IdleAction);
	}
}


void FUsingAbillityEnemyState::Enter(AMeleeAIController* Enemy)
{
	UCharacterAbillity* CurrentAbillity = Enemy->GetCurrentRunningAbillity();
	check(CurrentAbillity);

	CurrentAbillity->StartExecution(Enemy);
}

void FUsingAbillityEnemyState::Update(AMeleeAIController* Enemy)
{
	UCharacterAbillity* CurrentAbillity = Enemy->GetCurrentRunningAbillity();
	check(CurrentAbillity);

	if (CurrentAbillity->HasFinishedExecution())
	{
		CurrentAbillity->FinishExecution(Enemy);
		Enemy->SetCurrentRunningAbillity(nullptr);
		Enemy->SetCurrentState(CES_ChaseAction);
	}
	else
	{
		CurrentAbillity->Update(Enemy);
	}
}