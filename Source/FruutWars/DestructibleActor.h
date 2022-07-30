// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DestructibleActor.generated.h"

class UStaticMeshComponent;
class UHealthComponent;
class ULootDrop;

UCLASS()
class FRUUTWARS_API ADestructibleActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADestructibleActor();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UStaticMeshComponent* StaticMesh {};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		UHealthComponent* HealthComponent{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		ULootDrop* LootDrop {};

protected:

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void OnDeath();

};
