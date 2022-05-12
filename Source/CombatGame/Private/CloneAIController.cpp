#include "CloneAIController.h"

#include "CharacterBase.h"
#include "MeleeAIController.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"


ACloneAIController::ACloneAIController()
{
	AggroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("HearingSphere"));
	AggroSphere->CanCharacterStepUpOn = ECB_No;
	AggroSphere->bShouldCollideWhenPlacing = true;
	AggroSphere->SetCollisionProfileName("TriggerPawn");
	AggroSphere->SetCollisionObjectType(ECC_WorldDynamic);
	AggroSphere->OnComponentBeginOverlap.AddDynamic(this, &ACloneAIController::OnAggroOverlap);
	AggroSphere->OnComponentEndOverlap.AddDynamic(this, &ACloneAIController::OnAggroEndOverlap);
}

void ACloneAIController::OnFatalDamageTaken()
{
	// just die
	GetCharacter()->Destroy();
}

void ACloneAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Chase & Attack
	if (CurrentTarget)
	{
		if (CharacterBase->HasAnyCharactersToAttack() & CharacterBase->CanAttack())
		{
			CharacterBase->OnAttackStart_BP();
			CharacterBase->ExecuteAttack();
		}
		else
		{
			MoveToActor(CurrentTarget, CharacterBase->GetAttackRange());
		}
	}
	// Or Follow the Master
	else
	{
		FVector InstigatorLocation = AbillityInstigator->GetActorLocation();
		MoveToLocation(InstigatorLocation + FollowingOffset);
	}
}

void ACloneAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	CharacterBase = Cast<ACharacterBase>(InPawn);
	check(CharacterBase);

	AttachToActor(InPawn, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	AggroSphere->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	AggroSphere->SetSphereRadius(AggroRadius);
}

void ACloneAIController::OnAggroOverlap(UPrimitiveComponent*, AActor* Other, UPrimitiveComponent*, int32, bool, const FHitResult&)
{
	// Do not consider characters from the same race as enemies
	if (CharacterBase->GetClass() == Other->GetClass())
	{
		return;
	}
	checkf(Other != AbillityInstigator, TEXT("Abillity instigator must be of the same class as the controlled character"));

	ACharacterBase* PotentialTarget = Cast<ACharacterBase>(Other);
	if (!CurrentTarget && PotentialTarget)
	{
		CurrentTarget = PotentialTarget;
	}
}


void ACloneAIController::OnAggroEndOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int32)
{
	if (OtherActor == CurrentTarget)
	{
		CurrentTarget = nullptr;
	}
}
