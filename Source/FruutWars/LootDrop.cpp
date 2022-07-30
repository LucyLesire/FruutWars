// Fill out your copyright notice in the Description page of Project Settings.


#include "LootDrop.h"
#include "BaseWeapon.h"
#include "WeaponDrop.h"
#include "HealthComponent.h"
#include "BasePowerup.h"

// Sets default values for this component's properties
ULootDrop::ULootDrop()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	WeaponWeights.Add(TSubclassOf<ABaseWeapon>(), 50.f);
}

void ULootDrop::BeginPlay()
{
	Super::BeginPlay();

	if (auto health{ GetOwner()->FindComponentByClass<UHealthComponent>() })
	{
		health->OnDeath.AddDynamic(this, &ULootDrop::SpawnDrop);
	}
}

float ULootDrop::GetTotalWeight() const
{
	float totalWeight{};

	for (auto weaponWeight : WeaponWeights)
	{
		totalWeight += weaponWeight.Value;
	}

	for (auto powerUpWeight : PowerUpWeights)
	{
		totalWeight += powerUpWeight.Value;
	}

	return totalWeight;
}

void ULootDrop::SpawnDrop()
{
	float totalWeights = GetTotalWeight();
	if (totalWeights <= 0.0001f)
		return;
	float randomWeight = FMath::FRandRange(0, totalWeights);

	if (!GetOwner())
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red, TEXT("owner was nullptr"));
		return;
	}

	for (auto weaponWeight : WeaponWeights)
	{
		if (randomWeight > weaponWeight.Value)
			randomWeight -= weaponWeight.Value;
		else
		{
			auto transform = GetOwner()->GetTransform();
			FActorSpawnParameters spawnParam{};
			spawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			auto weaponDrop = GetWorld()->SpawnActor<AWeaponDrop>(spawnParam);
			if (weaponDrop)
				weaponDrop->SetActorTransform(transform);
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red, TEXT("Something went wrong spawning the weapon drop"));
				return;
			}

			auto weaponActor = GetWorld()->SpawnActor(weaponWeight.Key, &transform, spawnParam);
			if (!weaponActor)
			{
				GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red, TEXT("Something went wrong spawning the weapon actor"));
				return;
			}
			if (auto weapon{ Cast<ABaseWeapon>(weaponActor) })
				weaponDrop->SetWeapon(weapon);
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red, TEXT("spawned actor that could not be cast to a base weapon"));
			}

			return;
		}
	}

	for (auto PowerUpWeight : PowerUpWeights)
	{
		if (randomWeight > PowerUpWeight.Value)
			randomWeight -= PowerUpWeight.Value;
		else
		{
			auto transform = GetOwner()->GetTransform();
			FActorSpawnParameters spawnParam{};
			spawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			auto PowerUpActor = GetWorld()->SpawnActor(PowerUpWeight.Key, &transform, spawnParam);

			return;
		}
	}
}


