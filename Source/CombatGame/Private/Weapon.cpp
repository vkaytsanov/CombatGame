// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "Components/BoxComponent.h"
#include "Engine/SkeletalMeshSocket.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = Mesh;
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
}

void AWeapon::PostInitProperties()
{
	Super::PostInitProperties();
}

bool AWeapon::AttachToSocket(USkeletalMeshComponent* ActorMesh, const USkeletalMeshSocket* Socket)
{
	check(RootComponent);
	return Socket->AttachActor(this, ActorMesh);
}

