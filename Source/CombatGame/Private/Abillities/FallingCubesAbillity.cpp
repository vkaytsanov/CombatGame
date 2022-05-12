// Fill out your copyright notice in the Description page of Project Settings.


#include "Abillities/FallingCubesAbillity.h"

#include "CharacterBase.h"
#include "MeleeAIController.h"

void UFallingCubesAbillity::StartExecution(AController* Controller)
{
	Super::StartExecution(Controller);

	UWorld* ThisWorld = Controller->GetWorld();

	AMeleeAIController* AIController = Cast<AMeleeAIController>(Controller);
	if (AIController)
	{
		// Triangle Formation
		FVector Offsets[3] = {
			FVector(-300, 0, SpawnHeight),
			FVector(300, 0, SpawnHeight),
			FVector(0, 400, SpawnHeight)
		};

		const FVector& SpawnLocation = AIController->GetCurrentTarget()->GetActorLocation();
		for (const FVector& Offset : Offsets)
		{
			AActor* NewCube = ThisWorld->SpawnActor<AActor>(*CubeToSpawn, SpawnLocation + Offset, FRotator());
			UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(NewCube->GetRootComponent());
			check(Primitive);

			Primitive->SetEnableGravity(false);
			SpawnedCubes.Add(NewCube);
		}
		
	}

	FTimerHandle Handle;
	ThisWorld->GetTimerManager().SetTimer(Handle, this, &UFallingCubesAbillity::OnExecutionTimerEnd, MaxExecutionTime, false);
}

void UFallingCubesAbillity::OnExecutionTimerEnd()
{
	for (AActor* Cube : SpawnedCubes)
	{
		UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(Cube->GetRootComponent());
		check(Primitive);
		check(Primitive->IsAnySimulatingPhysics());

		Primitive->SetEnableGravity(true);
	}

	SpawnedCubes.Empty();
}
