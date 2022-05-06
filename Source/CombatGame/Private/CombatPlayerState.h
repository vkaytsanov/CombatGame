#pragma once

class ACombatPlayerController;
class AActor;


class FCombatPlayerState
{
public:
	virtual ~FCombatPlayerState() = default;
public:
	virtual void Enter(ACombatPlayerController* Player) = 0;
	virtual void Update(ACombatPlayerController* Player) = 0;
};


class FIdleActionState : public FCombatPlayerState
{
public:
	virtual void Enter(ACombatPlayerController* Player) override;
	virtual void Update(ACombatPlayerController* Player) override;
};


class FJumpActionState : public FCombatPlayerState
{
public:
	virtual void Enter(ACombatPlayerController* Player) override;
	virtual void Update(ACombatPlayerController* Player) override;
};

class FAttackActionState : public FCombatPlayerState
{
public:
	virtual void Enter(ACombatPlayerController* Player) override;
	virtual void Update(ACombatPlayerController* Player) override;
};


// Copyright Masashi Kishimoto, Naruto 
class FAmenotejikaraActionState : public FCombatPlayerState
{
	static constexpr float MaxTimeInThisStateInSeconds = 2.0f;
	static constexpr float TimeDilationWhenUsed = 0.05f;
	static constexpr float ToleranceToBeATargetInRadians = 3.1415926535897932f / 14;
public:
	virtual void Enter(ACombatPlayerController* Player) override;
	virtual void Update(ACombatPlayerController* Player) override;

	void AddActorInRange(AActor* InRangeActor)
	{
		ActorsInRange.Add(InRangeActor);
	}

	void RemoveActorOutOfRange(AActor* OutOfRangeActor)
	{
		ActorsInRange.RemoveSingleSwap(OutOfRangeActor);
	}
private:
	float EnterStateTime = FLT_MIN;

	TArray<AActor*> ActorsInRange;
};

