#pragma once

class AMeleeAIController;

class FCombatEnemyState
{
public:
	virtual ~FCombatEnemyState() = default;
public:
	virtual void Enter(AMeleeAIController* Enemy) = 0;
	virtual void Update(AMeleeAIController* Enemy) = 0;
};

class FIdleEnemyState : public FCombatEnemyState
{
public:
	virtual void Enter(AMeleeAIController* Enemy) override;
	virtual void Update(AMeleeAIController* Enemy) override;
private:
	float IdleTime;
};

class FWanderEnemyState : public FCombatEnemyState
{
public:
	virtual void Enter(AMeleeAIController* Enemy) override;
	virtual void Update(AMeleeAIController* Enemy) override;
};

class FChaseEnemyState : public FCombatEnemyState
{
public:
	virtual void Enter(AMeleeAIController* Enemy) override;
	virtual void Update(AMeleeAIController* Enemy) override;
};

class FAttackEnemyState : public FCombatEnemyState
{
public:
	virtual void Enter(AMeleeAIController* Enemy) override;
	virtual void Update(AMeleeAIController* Enemy) override;
};

class FReturnHomeEnemyState : public FCombatEnemyState
{
public:
	virtual void Enter(AMeleeAIController* Enemy) override;
	virtual void Update(AMeleeAIController* Enemy) override;
};

class FUsingAbillityEnemyState : public FCombatEnemyState
{
public:
	virtual void Enter(AMeleeAIController* Enemy) override;
	virtual void Update(AMeleeAIController* Enemy) override;
};
