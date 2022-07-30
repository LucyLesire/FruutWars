// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LootDrop.generated.h"

class ABaseWeapon;
class ABasePowerup;

/**
 * The Loot Drop Component will drop a weapon or power up when destroyed
 */
UCLASS( BlueprintType )
class FRUUTWARS_API ULootDrop : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULootDrop();

	virtual void BeginPlay() override;

protected:
	
	float GetTotalWeight() const;

	UFUNCTION()
	void SpawnDrop();

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<TSubclassOf<ABaseWeapon>, float> WeaponWeights{};

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<TSubclassOf<ABasePowerup>, float> PowerUpWeights{};
		
};
