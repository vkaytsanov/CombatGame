#pragma once
#include "AIController.h"
#include "Killable.h"
#include "CloneAIController.generated.h"


UCLASS()
class ACloneAIController : public AAIController, public IKillableInterface
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, Transient)
	class USphereComponent* AggroSphere;

	/* At what radius should the cloned actor be forced to go attacking enemies */
	UPROPERTY(EditAnywhere)
	float AggroRadius = 400.f;
public:
	ACloneAIController();
public:
	void SetAbillityInstigator(class ACharacterBase* InstigatorCharacter)
	{
		AbillityInstigator = InstigatorCharacter;
	}

	void SetFollowingOffset(const FVector& Offset)
	{
		FollowingOffset = Offset;
	}

	virtual void OnFatalDamageTaken() override;

	virtual void Tick(float DeltaTime) override;
protected:
	virtual void OnPossess(APawn* InPawn) override;
public:
	UFUNCTION()
	void OnAggroOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnAggroEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
private:
	UPROPERTY(Transient)
	class ACharacterBase* CharacterBase;

	UPROPERTY(Transient)
	class ACharacterBase* CurrentTarget;

	UPROPERTY(Transient)
	class ACharacterBase* AbillityInstigator;

	FVector FollowingOffset;
};
