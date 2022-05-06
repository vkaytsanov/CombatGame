#pragma once

class ACombatPlayerController;


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
public:
	virtual void Enter(ACombatPlayerController* Player) override;
	virtual void Update(ACombatPlayerController* Player) override;
private:
	float EnterStateTime = FLT_MIN;
	const float MaxStateTime = 2.0f;

};

