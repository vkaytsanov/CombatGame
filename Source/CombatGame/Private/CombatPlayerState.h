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


class FIdlePlayerState : public FCombatPlayerState
{
public:
	virtual void Enter(ACombatPlayerController* Player) override;
	virtual void Update(ACombatPlayerController* Player) override;
};


class FJumpPlayerState : public FCombatPlayerState
{
public:
	virtual void Enter(ACombatPlayerController* Player) override;
	virtual void Update(ACombatPlayerController* Player) override;
};

class FAttackPlayerState : public FCombatPlayerState
{
public:
	virtual void Enter(ACombatPlayerController* Player) override;
	virtual void Update(ACombatPlayerController* Player) override;

	bool bCanAdvanceAttackAfterNotifyForHit = false;
};

class FUsingAbillityPlayerState : public FCombatPlayerState
{
public:
	virtual void Enter(ACombatPlayerController* Player) override;
	virtual void Update(ACombatPlayerController* Player) override;
};
