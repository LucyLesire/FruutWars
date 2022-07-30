// Fill out your copyright notice in the Description page of Project Settings.


#include "DestructibleActor.h"
#include "Components/StaticMeshComponent.h"
#include "HealthComponent.h"
#include "LootDrop.h"

// Sets default values
ADestructibleActor::ADestructibleActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	SetRootComponent(StaticMesh);
	StaticMesh->SetCollisionProfileName(TEXT("Destructible"));

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));

	LootDrop = CreateDefaultSubobject<ULootDrop>(TEXT("Loot Drop"));

}

void ADestructibleActor::BeginPlay()
{
	Super::BeginPlay();

	HealthComponent->OnDeath.AddDynamic(this, &ADestructibleActor::OnDeath);
}

void ADestructibleActor::OnDeath_Implementation()
{
	Destroy();
}


